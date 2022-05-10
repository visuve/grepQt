#pragma once

#include <vector>

class Options;

struct Match
{
	std::pair<int32_t, int32_t> pos = { 0, 0 };
	std::pair<int32_t, int32_t> line = { 0, 0 };
	std::pair<char16_t*, char16_t*> ctx = { nullptr, nullptr };
};

class MatchDetector
{
public:
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
