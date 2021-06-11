#include "PCH.hpp"
#include "Options.hpp"

namespace Keys
{
	const QString Path = "path";
	const QString SearchExpression = "search/expression";
	const QString ReplacementText = "replace/text";
	const QString CaseSensitive = "search/casesensitive";
	const QString SearchMode = "search/mode";

	const QString Wildcards = "filter/wildcards";
	const QString Excludes = "filter/excludes";
	const QString SizeOption = "filter/size_option";
	const QString SizeValue = "filter/size_value";
	const QString TimeOption = "filter/time_option";
	const QString TimeValue = "filter/time_value";
}


Options::Options(QObject* parent) :
	QSettings(QSettings::IniFormat, QSettings::UserScope, "visuve", "grepQt", parent)
{
	_path = value(Keys::Path).value<QString>();
	_searchExpression = value(Keys::SearchExpression).value<QString>();
	_replacementText = value(Keys::ReplacementText).value<QString>();
	_isCaseSensitive = value(Keys::CaseSensitive, false).value<bool>();
	_searchMode = value(Keys::SearchMode, SearchMode::Plain).value<SearchMode>();
	_wildcards = value(Keys::Wildcards, "*.*").value<QString>().split('|');
	_excludes = value(Keys::Excludes, ".git").value<QString>().split('|');
	_sizeFilterOption = value(Keys::SizeOption, 0).value<ComparisonOption>();
	_sizeFilterValue = value(Keys::SizeValue, 4).value<qint64>();
	_timeFilterOption = value(Keys::TimeOption, 0).value<ComparisonOption>();
	_timeFilterValue = QDateTime::fromSecsSinceEpoch(value(Keys::TimeValue, 1623342562).value<qint64>());
}

Options::~Options()
{
	setValue(Keys::Path, _path);
	setValue(Keys::SearchExpression, _searchExpression);
	setValue(Keys::ReplacementText, _replacementText );
	setValue(Keys::CaseSensitive, _isCaseSensitive);
	setValue(Keys::SearchMode, _searchMode);
	setValue(Keys::Wildcards, _wildcards.join('|'));
	setValue(Keys::Excludes, _excludes.join('|'));
	setValue(Keys::SizeOption, _sizeFilterOption);
	setValue(Keys::SizeValue, _sizeFilterValue);
	setValue(Keys::TimeOption, _timeFilterOption);
	setValue(Keys::TimeValue, _timeFilterValue.toSecsSinceEpoch());
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
		setValue(Keys::SizeOption, value);
	}
}

qint64 Options::sizeFilterValue() const
{
	qDebug() << _sizeFilterValue;
	return _sizeFilterValue;
}

void Options::setSizeFilterValue(qint64 value)
{
	if (_sizeFilterValue != value)
	{
		qDebug() << _sizeFilterValue << "->" << value;
		_sizeFilterValue = value;
		setValue(Keys::SizeValue, value);
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
		setValue(Keys::TimeOption, value);
	}
}

const QDateTime& Options::timeFilterValue() const
{
	qDebug() << _timeFilterValue;
	return _timeFilterValue;
}

void Options::setTimeFilterValue(const QDateTime& value)
{
	if (_timeFilterValue != value)
	{
		qDebug() << _timeFilterValue << "->" << value;
		_timeFilterValue = value;
		setValue(Keys::TimeValue, value.toSecsSinceEpoch());
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
				sizeMatches = fileInfo.size() < _sizeFilterValue;
				break;
			case Options::ComparisonOption::Greater:
				sizeMatches = fileInfo.size() > _sizeFilterValue;
				break;
			case Options::ComparisonOption::Equals:
				sizeMatches = fileInfo.size() == _sizeFilterValue;
				break;
		}

		bool lastModifiedMatches = false;

		switch (_timeFilterOption)
		{
			case Options::ComparisonOption::Irrelevant:
				lastModifiedMatches = true;
				break;
			case Options::ComparisonOption::Lesser:
				sizeMatches = fileInfo.lastModified() < _timeFilterValue;
				break;
			case Options::ComparisonOption::Greater:
				sizeMatches = fileInfo.lastModified() > _timeFilterValue;
				break;
			case Options::ComparisonOption::Equals:
				sizeMatches = fileInfo.lastModified() == _timeFilterValue;
				break;
		}

		return sizeMatches && lastModifiedMatches;
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
