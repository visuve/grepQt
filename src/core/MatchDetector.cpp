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
	char16_t* target = buffer.data();
	char16_t* targetLimit = buffer.data() + buffer.size();

	const char* sourceData = content.data();
	const char* sourceDataLimit = content.data() + content.size();

	ucnv_toUnicode(_converter, &target, targetLimit, &sourceData, sourceDataLimit, nullptr, flush, &_status);

	if (U_FAILURE(_status))
	{
		qWarning() << "ucnv_toUnicode failed";
		return false;
	}

	size_t nulls = targetLimit - target;
	_haystack.append(buffer, 0, buffer.size() - nulls);

	if (flush && find())
	{
		return setLines();
	}

	return true;
}

bool MatchDetector::find()
{
	char16_t* target = _haystack.data();
	int32_t haystackSize = static_cast<int32_t>(_haystack.size());

	uregex_setText(_regex, target, haystackSize, &_status);

	if (U_FAILURE(_status))
	{
		qWarning() << "uregex_setText failed";
		return false;
	}

	while (uregex_findNext(_regex, &_status))
	{
		Match m;

		m.pos = std::make_pair<int32_t, int32_t>(
			uregex_start(_regex, 0, &_status),
			uregex_end(_regex, 0, &_status));

		_matches.push_back(m);
	}

	return !_matches.empty();
}


bool MatchDetector::setLines()
{
	char16_t* haystack = _haystack.data();
	int32_t haystackSize = static_cast<int32_t>(_haystack.size());

	ubrk_setText(_iterator, haystack, haystackSize, &_status);

	if (U_FAILURE(_status))
	{
		qWarning() << "ubrk_setText failed";
		return false;
	}

	std::pair<int32_t, int32_t> pos;
	pos.first = ubrk_first(_iterator);
	pos.second = ubrk_next(_iterator);

	const auto startPositionMatches = [&](const Match& m)->bool
	{
		return pos.first <= m.pos.first && pos.second >= m.pos.first;
	};

	const auto endPositionMatches = [&](const Match& m)->bool
	{
		return pos.first <= m.pos.second && pos.second >= m.pos.second;
	};

	for (int32_t line = 1; pos.first != UBRK_DONE && pos.second != UBRK_DONE; ++line)
	{
		{
			auto match = std::find_if(_matches.begin(), _matches.end(), startPositionMatches);

			if (match != _matches.end())
			{
				match->line.first = line;
				match->ctx.first = haystack + pos.first;
			}
		}
		{
			auto match = std::find_if(_matches.begin(), _matches.end(), endPositionMatches);

			if (match != _matches.end())
			{
				match->line.second = line;
				match->ctx.second = haystack + pos.second;
			}
		}

		pos.first = ubrk_current(_iterator);
		pos.second = ubrk_next(_iterator);
	}

	return true;
}
