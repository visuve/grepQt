#include "Options.hpp"

namespace Keys
{
	constexpr char Path[] = "target/path";
	constexpr char Wildcards[] = "target/wildcards";
	constexpr char Excludes[] = "target/excludes";

	constexpr char SearchExpression[] = "search/expression";
	constexpr char ReplacementText[] = "search/replacement";
	constexpr char CaseSensitive[] = "search/case_sensitive";
	constexpr char SearchMode[] = "search/mode";

	constexpr char SizeOption[] = "filter/size_option";
	constexpr char SizeFrom[] = "filter/size_from";
	constexpr char SizeTo[] = "filter/size_to";

	constexpr char TimeOption[] = "filter/time_option";
	constexpr char TimeFrom[] = "filter/time_from";
	constexpr char TimeTo[] = "filter/time_to";
}

Options::Options(const QString& application, QObject* parent) :
	QSettings(QSettings::IniFormat, QSettings::UserScope, "visuve", application, parent)
{
	_path = value(Keys::Path).value<QString>();
	_wildcards = value(Keys::Wildcards, "*.*").value<QString>().split('|');
	_excludes = value(Keys::Excludes, ".bzr|.git|.hg|.svn").value<QString>().split('|');

	_searchExpression = value(Keys::SearchExpression).value<QString>();
	_replacementText = value(Keys::ReplacementText).value<QString>();
	_isCaseSensitive = value(Keys::CaseSensitive, false).value<bool>();
	_searchMode = value(Keys::SearchMode, SearchMode::Plain).value<SearchMode>();

	_sizeFilterOption = static_cast<ComparisonOption>(value(Keys::SizeOption, 0).value<int>());
	_sizeFilterFrom = value(Keys::SizeFrom, 1).value<qint64>();
	_sizeFilterTo = value(Keys::SizeTo, 10).value<qint64>();

	_timeFilterOption = static_cast<ComparisonOption>(value(Keys::TimeOption, 0).value<int>());
	_timeFilterFrom = QDateTime::fromSecsSinceEpoch(value(Keys::TimeFrom, 1623342562).value<qint64>());
	_timeFilterTo = QDateTime::fromSecsSinceEpoch(value(Keys::TimeTo, 1623397338).value<qint64>());
}

Options::Options(QObject* parent) :
	Options("grepQt", parent)
{
}

Options::~Options()
{
	qDebug() << "Destroying...";

	setValue(Keys::Path, _path);
	setValue(Keys::Wildcards, _wildcards.join('|'));
	setValue(Keys::Excludes, _excludes.join('|'));

	setValue(Keys::SearchExpression, _searchExpression);
	setValue(Keys::ReplacementText, _replacementText);

	setValue(Keys::CaseSensitive, _isCaseSensitive);
	setValue(Keys::SearchMode, _searchMode);

	setValue(Keys::SizeOption, static_cast<int>(_sizeFilterOption));
	setValue(Keys::SizeFrom, _sizeFilterFrom);
	setValue(Keys::SizeTo, _sizeFilterTo);

	setValue(Keys::TimeOption, static_cast<int>(_timeFilterOption));
	setValue(Keys::TimeFrom, _timeFilterFrom.toSecsSinceEpoch());
	setValue(Keys::TimeTo, _timeFilterTo.toSecsSinceEpoch());

	sync(); // TODO: probably not needed

	qDebug() << "Destroyed";
}

const QString& Options::path() const
{
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

void Options::setWildcards(const QString& wildcards)
{
	if (wildcards.isEmpty())
	{
		_wildcards.clear();
	}
	else
	{
		setWildcards(wildcards.split('|'));
	}
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
	return _excludes;
}

void Options::setExcludes(const QString& excludes)
{
	if (excludes.isEmpty())
	{
		_excludes.clear();
	}
	else
	{
		setExcludes(excludes.split('|'));
	}
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

Options::ComparisonOption Options::timeFilterOption() const
{
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
				sizeMatches = fileInfo.size() < _sizeFilterTo;
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
				lastModifiedMatches = fileInfo.lastModified() < _timeFilterTo;
				break;
			case Options::ComparisonOption::Greater:
				lastModifiedMatches = fileInfo.lastModified() > _timeFilterFrom;
				break;

			case Options::ComparisonOption::Between:
				lastModifiedMatches = fileInfo.lastModified() > _timeFilterFrom && fileInfo.lastModified() < _timeFilterTo;
				break;
		}

		return sizeMatches && lastModifiedMatches;
	};
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
