#pragma once

#include <vector>

class Options;

class MatchDetector
{
public:
	struct Match
	{
		int32_t start = 0;
		int32_t end = 0;
		int32_t line = 0;

		std::strong_ordering operator<=>(const Match& m) const
		{
			return std::tie(start, end, line) <=>
				std::tie(m.start, m.end, m.line);
		}

		std::u16string content;
	};

	MatchDetector(const Options& options, const QString& encoding);

	~MatchDetector();

	bool feed(QByteArrayView content, bool flush);

	inline std::vector<Match> matches() const
	{
		return _matches;
	}

	inline auto begin() const
	{
		return _matches.cbegin();
	}

	inline auto end() const
	{
		return _matches.cend();
	}

private:
	bool find();
	bool setLines();

	UErrorCode _status = U_ZERO_ERROR;
	UConverter* _converter = nullptr;
	UBreakIterator* _iterator = nullptr;
	URegularExpression* _regex = nullptr;

	std::u16string _haystack;

	int _line = 0;
	std::vector<Match> _matches;
};
