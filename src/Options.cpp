#include "Options.hpp"

namespace Keys
{
	const QString Path = "target/path";
	const QString Wildcards = "target/wildcards";
	const QString Excludes = "target/excludes";

	const QString SearchExpression = "search/expression";
	const QString ReplacementText = "search/replacement";
	const QString CaseSensitive = "search/case_sensitive";
	const QString SearchMode = "search/mode";

	const QString SizeOption = "filter/size_option";
	const QString SizeFrom = "filter/size_from";
	const QString SizeTo = "filter/size_to";

	const QString EntropySensitive = "filter/entropy_sensitive";
	const QString EntropyLimit = "filter/entropy_limit";

	const QString TimeOption = "filter/time_option";
	const QString TimeFrom = "filter/time_from";
	const QString TimeTo = "filter/time_to";

	const QString ResultMode = "result/mode";
}

Options::Options(QObject* parent) :
	QSettings(QSettings::IniFormat, QSettings::UserScope, "visuve", "grepQt", parent)
{
	_path = value(Keys::Path).value<QString>();
	_wildcards = value(Keys::Wildcards, "*.*").value<QString>().split('|');
	_excludes = value(Keys::Excludes, ".git").value<QString>().split('|');

	_searchExpression = value(Keys::SearchExpression).value<QString>();
	_replacementText = value(Keys::ReplacementText).value<QString>();
	_isCaseSensitive = value(Keys::CaseSensitive, false).value<bool>();
	_searchMode = value(Keys::SearchMode, SearchMode::Plain).value<SearchMode>();

	_sizeFilterOption = static_cast<ComparisonOption>(value(Keys::SizeOption, 0).value<int>());
	_sizeFilterFrom = value(Keys::SizeFrom, 1).value<qint64>();
	_sizeFilterTo = value(Keys::SizeTo, 10).value<qint64>();

	_isEntropySensitive = value(Keys::EntropySensitive, false).value<bool>();
	_entropyLimit = value(Keys::EntropyLimit, 4.5).value<double>();

	_timeFilterOption = static_cast<ComparisonOption>(value(Keys::TimeOption, 0).value<int>());
	_timeFilterFrom = QDateTime::fromSecsSinceEpoch(value(Keys::TimeFrom, 1623342562).value<qint64>());
	_timeFilterTo = QDateTime::fromSecsSinceEpoch(value(Keys::TimeTo, 1623397338).value<qint64>());

	_resultMode = value(Keys::ResultMode, ResultMode::ShowContent).value<ResultMode>();
}

Options::~Options()
{
	setValue(Keys::Path, _path);
	setValue(Keys::Wildcards, _wildcards.join('|'));
	setValue(Keys::Excludes, _excludes.join('|'));

	setValue(Keys::SearchExpression, _searchExpression);
	setValue(Keys::ReplacementText, _replacementText );
	setValue(Keys::CaseSensitive, _isCaseSensitive);
	setValue(Keys::SearchMode, _searchMode);

	setValue(Keys::SizeOption, static_cast<int>(_sizeFilterOption));
	setValue(Keys::SizeFrom, _sizeFilterFrom);
	setValue(Keys::SizeTo, _sizeFilterTo);

	setValue(Keys::EntropySensitive, _isEntropySensitive);
	setValue(Keys::EntropyLimit, _entropyLimit);

	setValue(Keys::TimeOption, static_cast<int>(_timeFilterOption));
	setValue(Keys::TimeFrom, _timeFilterFrom.toSecsSinceEpoch());
	setValue(Keys::TimeTo, _timeFilterTo.toSecsSinceEpoch());

	setValue(Keys::ResultMode, static_cast<int>(_resultMode));

	sync(); // TODO: probably not needed
}

const QString& Options::path() const
{
	qDebug() << _path;
	return _path;
}

void Options::setPath(const QString& value)
{
	if (_path != value)
	{
		qDebug() << _path << "->" << value;
		_path = value;
		setValue(Keys::Path, value);
	}
}

const QStringList& Options::wildcards() const
{
	qDebug() << _wildcards;
	return _wildcards;
}

void Options::setWildcards(const QStringList& value)
{
	if (_wildcards != value)
	{
		qDebug() << _wildcards << "->" << value;
		_wildcards = value;
		setValue(Keys::Wildcards, value.join('|'));
	}
}

const QStringList& Options::excludes() const
{
	qDebug() << _excludes;
	return _excludes;
}

void Options::setExcludes(const QStringList& value)
{
	if (_excludes != value)
	{
		qDebug() << _excludes << "->" << value;
		_excludes = value;
		setValue(Keys::Excludes, value.join('|'));
	}
}

const QString& Options::searchExpression() const
{
	qDebug() << _searchExpression;
	return _searchExpression;
}

void Options::setSearchExpression(const QString& value)
{
	if (_searchExpression != value)
	{
		qDebug() << _searchExpression << "->" << value;
		_searchExpression = value;
		setValue(Keys::SearchExpression, value);
	}
}

const QString& Options::replacementText() const
{
	qDebug() << _replacementText;
	return _replacementText;
}

void Options::setReplacementText(const QString& value)
{
	if (_replacementText != value)
	{
		qDebug() << _replacementText << "->" << value;
		_replacementText = value;
		setValue(Keys::ReplacementText, value);
	}
}

Options::SearchMode Options::searchMode() const
{
	qDebug() << _searchMode;
	return _searchMode;
}

void Options::setSearchMode(SearchMode value)
{
	if (_searchMode != value)
	{
		qDebug() << _searchMode << "->" << value;
		_searchMode = value;
		setValue(Keys::SearchMode, value);
	}
}

bool Options::isCaseSensitive() const
{
	qDebug() << _isCaseSensitive;
	return _isCaseSensitive;
}

void Options::setCaseSensitive(bool value)
{
	if (_isCaseSensitive != value)
	{
		qDebug() << _isCaseSensitive << "->" << value;
		_isCaseSensitive = value;
		setValue(Keys::CaseSensitive, value);
	}
}

Options::ComparisonOption Options::sizeFilterOption() const
{
	qDebug() << _sizeFilterOption;
	return _sizeFilterOption;
}

void Options::setSizeFilterOption(ComparisonOption value)
{
	if (_sizeFilterOption != value)
	{
		qDebug() << _sizeFilterOption << "->" << value;
		_sizeFilterOption = value;
		setValue(Keys::SizeOption, static_cast<int>(value));
	}
}

qint64 Options::sizeFilterFrom() const
{
	qDebug() << _sizeFilterFrom;
	return _sizeFilterFrom;
}

void Options::setSizeFilterFrom(qint64 value)
{
	if (_sizeFilterFrom != value)
	{
		qDebug() << _sizeFilterFrom << "->" << value;
		_sizeFilterFrom = value;
		setValue(Keys::SizeFrom, value);
	}
}

qint64 Options::sizeFilterTo() const
{
	qDebug() << _sizeFilterTo;
	return _sizeFilterTo;
}

void Options::setSizeFilterTo(qint64 value)
{
	if (_sizeFilterTo != value)
	{
		qDebug() << _sizeFilterTo<< "->" << value;
		_sizeFilterTo = value;
		setValue(Keys::SizeTo, value);
	}
}

bool Options::isEntropySensitive() const
{
	qDebug() << _isEntropySensitive;
	return _isEntropySensitive;
}

void Options::setEntropySensitive(bool value)
{
	if (_isEntropySensitive != value)
	{
		qDebug() << _isEntropySensitive << "->" << value;
		_isEntropySensitive = value;
		setValue(Keys::EntropySensitive, value);
	}
}

double Options::entropyLimit() const
{
	qDebug() << _entropyLimit;
	return _entropyLimit;
}

void Options::setEntropyLimit(double value)
{
	if (_entropyLimit != value)
	{
		qDebug() << _entropyLimit << "->" << value;
		_entropyLimit = value;
		setValue(Keys::EntropyLimit, value);
	}
}

Options::ComparisonOption Options::timeFilterOption() const
{
	qDebug() << _timeFilterOption;
	return _timeFilterOption;
}

void Options::setTimeFilterOption(ComparisonOption value)
{
	if (_timeFilterOption != value)
	{
		qDebug() << _timeFilterOption << "->" << value;
		_timeFilterOption = value;
		setValue(Keys::TimeOption, static_cast<int>(value));
	}
}

const QDateTime& Options::timeFilterFrom() const
{
	qDebug() << _timeFilterFrom;
	return _timeFilterFrom;
}

void Options::setTimeFilterFrom(const QDateTime& value)
{
	if (_timeFilterFrom != value)
	{
		qDebug() << _timeFilterFrom << "->" << value;
		_timeFilterFrom = value;
		setValue(Keys::TimeFrom, value.toSecsSinceEpoch());
	}
}

const QDateTime& Options::timeFilterTo() const
{
	qDebug() << _timeFilterTo;
	return _timeFilterTo;
}

void Options::setTimeFilterTo(const QDateTime& value)
{
	if (_timeFilterTo != value)
	{
		qDebug() << _timeFilterTo << "->" << value;
		_timeFilterTo = value;
		setValue(Keys::TimeTo, value.toSecsSinceEpoch());
	}
}

Options::ResultMode Options::resultMode() const
{
	qDebug() << _resultMode;
	return _resultMode;
}

void Options::setResultMode(ResultMode value)
{
	if (_resultMode != value)
	{
		qDebug() << _resultMode << "->" << value;
		_resultMode = value;
		setValue(Keys::ResultMode, value);
	}
}

std::function<bool (const QFileInfo&)> Options::createFilterFunction() const
{
	return [&](const QFileInfo& fileInfo)
	{
		const QString dirName = fileInfo.dir().dirName();

		for (const QString& excluded : _excludes)
		{
			if (dirName == excluded)
			{
				return false;
			}
		}

		bool sizeMatches = false;

		switch (_sizeFilterOption)
		{
			case Options::ComparisonOption::Irrelevant:
				sizeMatches = true;
				break;
			case Options::ComparisonOption::Lesser:
				sizeMatches = fileInfo.size() < _sizeFilterFrom;
				break;
			case Options::ComparisonOption::Greater:
				sizeMatches = fileInfo.size() > _sizeFilterFrom;
				break;
			case Options::ComparisonOption::Between:
				sizeMatches = fileInfo.size() > _sizeFilterFrom && fileInfo.size() < _sizeFilterTo;
				break;
		}

		bool lastModifiedMatches = false;

		switch (_timeFilterOption)
		{
			case Options::ComparisonOption::Irrelevant:
				lastModifiedMatches = true;
				break;
			case Options::ComparisonOption::Lesser:
				sizeMatches = fileInfo.lastModified() < _timeFilterFrom;
				break;
			case Options::ComparisonOption::Greater:
				sizeMatches = fileInfo.lastModified() > _timeFilterFrom;
				break;
			case Options::ComparisonOption::Between:
				sizeMatches = fileInfo.lastModified() > _timeFilterFrom && fileInfo.lastModified() < _timeFilterTo;
				break;
		}

		return sizeMatches && lastModifiedMatches;
	};
}

std::function<bool (QStringView)> Options::createBreakFunction() const
{
	return [&](QStringView line)->bool
	{
		if (_entropyLimit)
		{
			QMap<QChar, double> frequencies;

			for (QChar x : line)
			{
				++frequencies[x];
			}

			double entropy = 0;

			for (double value : frequencies)
			{
				double frequency = value / line.size();
				entropy -= frequency * std::log2(frequency);
			}

			if (entropy >= _entropyLimit)
			{
				qDebug() << "Too high entropy: " << entropy << "skipping";
				return true;
			}
		}

		return false; // Don't break
	};
}

std::function<bool(QStringView)> Options::createMatchFunction() const
{
	switch (_searchMode)
	{
		case Options::SearchMode::Plain:
		{
			const Qt::CaseSensitivity caseSensitivity = _isCaseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;

			return [&](QStringView haystack)->bool
			{
				return haystack.contains(_searchExpression, caseSensitivity);
			};
		}
		case Options::SearchMode::Regex:
		{
			const QRegularExpression::PatternOptions options = !_isCaseSensitive ?
				QRegularExpression::DontCaptureOption | QRegularExpression::CaseInsensitiveOption :
				QRegularExpression::DontCaptureOption;

			const QRegularExpression regex(_searchExpression, options);
			regex.optimize();

			return [&](QStringView haystack)->bool
			{
				return haystack.contains(regex);
			};
		}
	}

	return nullptr;
}

std::function<bool(QString&)> Options::createReplaceFunction() const
{
	switch (_searchMode)
	{
		case Options::SearchMode::Plain:
		{
			const Qt::CaseSensitivity caseSensitivity = _isCaseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;

			return [&](QString& line)->bool
			{
				const QString before(line);
				return line.replace(_searchExpression, _replacementText, caseSensitivity) != before;
			};
		}
		case Options::SearchMode::Regex:
		{
			const QRegularExpression::PatternOptions options = !_isCaseSensitive ?
					QRegularExpression::CaseInsensitiveOption :
					QRegularExpression::NoPatternOption;

			const QRegularExpression regex(_searchExpression, options);
			regex.optimize();

			return [&](QString& line)->bool
			{
				const QString before(line);
				return line.replace(regex, _replacementText) != before;
			};
		}
	}

	return nullptr;
}
