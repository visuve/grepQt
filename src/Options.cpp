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
	_sizeFilterOption = value(Keys::SizeOption, 0).value<ComparisonOption>();
	_sizeFilterValue = value(Keys::SizeValue, 4).value<int>();
	_timeFilterOption = value(Keys::TimeOption, 0).value<ComparisonOption>();
	_timeFilterValue = QDateTime().addSecs(value(Keys::TimeValue, 1623342562).value<int>());
}

Options::~Options()
{
	qDebug();
}

const QString& Options::path() const
{
	qDebug();
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
	qDebug();
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
	qDebug();
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
	qDebug();
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
	qDebug();
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
	qDebug();
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

Options::ComparisonOption Options::sizeFilterOptions() const
{
	qDebug();
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

int Options::sizeFilterValue() const
{
	qDebug();
	return _sizeFilterValue;
}

void Options::setSizeFilterValue(int value)
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
	qDebug();
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

const QDateTime& Options::timeFilterValue()
{
	qDebug();
	return _timeFilterValue;
}

void Options::setTimeFilterValue(const QDateTime& value)
{
	if (_timeFilterValue != value)
	{
		qDebug() << _timeFilterValue << "->" << value;
		_timeFilterValue = value;
		setValue(Keys::TimeValue, value.currentSecsSinceEpoch());
	}
}
