#include "MatchDetector.hpp"
#include "Options.hpp"

MatchDetector::MatchDetector(const Options& options, const QString& encoding)
{
	_converter = ucnv_open(qPrintable(encoding), &_status);

	if (U_FAILURE(_status))
	{
		qWarning() << "ucnv_open failed";
		return;
	}

	_iterator = ubrk_open(UBRK_SENTENCE, nullptr, nullptr, 0, &_status);

	if (U_FAILURE(_status))
	{
		qWarning() << "ubrk_open failed";
		return;
	}

	int32_t flags = options.searchMode() == Options::Plain ? UREGEX_LITERAL : 0;

	if (!options.isCaseSensitive())
	{
		flags |= UREGEX_CASE_INSENSITIVE;
	}

	auto needle = options.searchExpression().toStdU16String();
	int32_t needleSize = static_cast<int32_t>(needle.size());

	UParseError error;
	_regex = uregex_open(needle.c_str(), needleSize, flags, &error, &_status);

	if (U_FAILURE(_status))
	{
		qWarning() << "uregex_open failed";
		return;
	}
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

bool MatchDetector::feed(QByteArrayView content, bool flush)
{
	if (U_FAILURE(_status))
	{
		return false;
	}

	std::u16string buffer(content.size(), '\0');

	UChar* target = buffer.data();
	UChar* targetLimit = buffer.data() + buffer.size();

	const char* sourceData = content.data();
	const char* sourceDataLimit = content.data() + content.size();

	ucnv_toUnicode(_converter, &target, targetLimit, &sourceData, sourceDataLimit, nullptr, flush, &_status);

	if (U_FAILURE(_status))
	{
		qWarning() << "ucnv_toUnicode failed";
		return false;
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

	return detectMatch();
}

bool MatchDetector::detectMatch()
{
	UChar* haystack = _haystack.data();
	int32_t haystackSize = static_cast<int32_t>(_haystack.size());

	ubrk_setText(_iterator, haystack, haystackSize, &_status);

	if (U_FAILURE(_status))
	{
		qWarning() << "ubrk_setText failed";
		return false;
	}

	int32_t prev = ubrk_first(_iterator);
	int32_t next = ubrk_next(_iterator);

	while (prev >= 0 && next > 0)
	{
		++_line;
		
		uregex_setText(_regex, haystack + prev, next - prev, &_status);

		if (U_FAILURE(_status))
		{
			qWarning() << "uregex_setText failed";
			return false;
		}

		bool found = uregex_find(_regex, 0, &_status);

		if (U_FAILURE(_status))
		{
			qWarning() << "uregex_find failed";
			return false;
		}

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

	// The sample had lines
	return _line > 0;
}
