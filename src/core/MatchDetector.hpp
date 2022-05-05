#pragma once

class Options;

class MatchDetector
{
public:
	MatchDetector(const Options& options, const QString& encoding);

	~MatchDetector();

	bool feed(QByteArrayView content, bool flush);

	inline auto begin() const
	{
		return _matches.cbegin();
	}

	inline auto end() const
	{
		return _matches.cend();
	}

private:
	// Returns false if the sample contained no lines
	bool detectMatch();

	UErrorCode _status = U_ZERO_ERROR;
	UConverter* _converter = nullptr;
	UBreakIterator* _iterator = nullptr;
	URegularExpression* _regex = nullptr;

	std::u16string _haystack;

	int _line = 0;
	std::map<int, std::u16string> _matches;
};
