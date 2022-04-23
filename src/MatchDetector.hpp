#pragma once

class Options;

class MatchDetector
{
public:
	MatchDetector(const Options& options, const QString& encoding);

	~MatchDetector();

	void feed(const char* data, size_t size, bool flush);

	inline auto begin() const
	{
		return _matches.cbegin();
	}

	inline auto end() const
	{
		return _matches.cend();
	}

private:
	void detectMatch();

	UConverter* _converter = nullptr;
	UBreakIterator* _iterator = nullptr;
	URegularExpression* _regex = nullptr;

	std::u16string _haystack;

	int _line = 0;
	std::map<int, std::u16string> _matches;
};
