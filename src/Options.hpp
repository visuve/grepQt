#pragma once

#include <QSettings>
#include <QObject>

class Options : public QSettings
{
	Q_OBJECT
public:
	enum SearchMode : char
	{
		Plain = 'p',
		Regex = 'r'
	};

	enum ComparisonOption : int
	{
		Irrelevant = 0,
		SmallerThan = 1,
		LargerThan = 2,
		Equals = 3
	};

	explicit Options(QObject *parent = nullptr);
	~Options();

	const QString& path() const;
	void setPath(const QString& path);

	const QString& searchExpression() const;
	void setSearchExpression(const QString& value);

	const QString& replacementText() const;
	void setReplacementText(const QString& value);

	bool isCaseSensitive() const;
	void setCaseSensitive(bool value);

	SearchMode searchMode() const;
	void setSearchMode(SearchMode value);

	const QStringList& wildcards() const;
	void setWildcards(const QStringList& value);

	ComparisonOption sizeFilterOptions() const;
	void setSizeFilterOption(ComparisonOption value);

	int sizeFilterValue() const;
	void setSizeFilterValue(int value);

	ComparisonOption timeFilterOption() const;
	void setTimeFilterOption(ComparisonOption value);

	const QDateTime& timeFilterValue();
	void setTimeFilterValue(const QDateTime& value);

private:
	QString _path;
	QString _searchExpression;
	QString _replacementText;
	QStringList _wildcards;
	bool _isCaseSensitive;
	SearchMode _searchMode;
	ComparisonOption _sizeFilterOption;
	int _sizeFilterValue;
	ComparisonOption _timeFilterOption;
	QDateTime _timeFilterValue;
};
