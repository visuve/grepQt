#include "MatchDetector.hpp"
#include "Options.hpp"

MatchDetector::MatchDetector(const Options& options, const QString& encoding)
{
	UErrorCode status = U_ZERO_ERROR;
	_converter = ucnv_open(qPrintable(encoding), &status);
	Q_ASSERT(U_SUCCESS(status));

	_iterator = ubrk_open(UBRK_SENTENCE, nullptr, nullptr, 0, &status);
	Q_ASSERT(U_SUCCESS(status));

	int32_t flags = options.searchMode() == Options::Plain ? UREGEX_LITERAL : 0;

	if (!options.isCaseSensitive())
	{
		flags |= UREGEX_CASE_INSENSITIVE;
	}

	auto needle = options.searchExpression().toStdU16String();
	int32_t needleSize = static_cast<int32_t>(needle.size());

	UParseError error;
	_regex = uregex_open(needle.c_str(), needleSize, flags, &error, &status);
	Q_ASSERT(U_SUCCESS(status));
}

MatchDetector::~MatchDetector()
{
	if (_converter)
	{
		ucnv_close(_converter);
	}

	if (_iterator)
	{
		ubrk_close(_iterator);
	}

	if (_regex)
	{
		uregex_close(_regex);
	}
}

void MatchDetector::feed(const char* data, size_t size, bool flush)
{
	std::u16string buffer(size, '\0');

	UChar* target = buffer.data();
	UChar* targetLimit = target + buffer.size();

	const char* sourceDataLimit = data + size;

	UErrorCode status = U_ZERO_ERROR;
	ucnv_toUnicode(_converter, &target, targetLimit, &data, sourceDataLimit, nullptr, flush, &status);

	if (U_FAILURE(status))
	{
		qWarning() << "Conversion failure";
		return;
	}

	size_t lastNull = buffer.find_last_not_of(u'\0') + 1;

	if (lastNull < buffer.size())
	{
		_haystack += buffer.substr(0, lastNull);
	}
	else
	{
		_haystack += buffer;
	}

	detectMatch();
}

void MatchDetector::detectMatch()
{
	UChar* haystack = _haystack.data();
	int32_t haystackSize = static_cast<int32_t>(_haystack.size());

	UErrorCode status = U_ZERO_ERROR;
	ubrk_setText(_iterator, haystack, haystackSize, &status);
	Q_ASSERT(U_SUCCESS(status));

	int32_t prev = ubrk_first(_iterator);
	int32_t next = ubrk_next(_iterator);

	while (prev >= 0 && next > 0)
	{
		++_line;
		
		uregex_setText(_regex, haystack + prev, next - prev, &status);
		Q_ASSERT(status == U_ZERO_ERROR);

		bool found = uregex_find(_regex, 0, &status);
		Q_ASSERT(status == U_ZERO_ERROR);

		if (found)
		{
			_matches[_line] = _haystack.substr(prev, next - prev);
			qDebug() << "Found: " << _matches[_line];
		}

		prev = next;
		next = ubrk_next(_iterator);
	}

	if (prev > 0)
	{
		prev = ubrk_previous(_iterator);

		if (prev > 0)
		{
			_haystack.erase(0, prev);
		}
	}
}
