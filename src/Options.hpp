#pragma once

#include <QSettings>
#include <QObject>

class Options : public QSettings
{
	Q_OBJECT
public:
	enum SearchMode : int
	{
		Plain = 0,
		Regex = 1
	};

	enum ComparisonOption : int
	{
		Irrelevant = 0,
		Lesser = 1,
		Greater = 2,
		Equals = 3
	};

	explicit Options(QObject *parent = nullptr);
	~Options();

	const QString& path() const;
	void setPath(const QString& path);

	const QStringList& wildcards() const;
	void setWildcards(const QStringList& value);

	const QStringList& excludes() const;
	void setExcludes(const QStringList& value);

	const QString& searchExpression() const;
	void setSearchExpression(const QString& value);

	const QString& replacementText() const;
	void setReplacementText(const QString& value);

	bool isCaseSensitive() const;
	void setCaseSensitive(bool value);

	SearchMode searchMode() const;
	void setSearchMode(SearchMode value);

	ComparisonOption sizeFilterOption() const;
	void setSizeFilterOption(ComparisonOption value);

	qint64 sizeFilterValue() const;
	void setSizeFilterValue(qint64 value);

	ComparisonOption timeFilterOption() const;
	void setTimeFilterOption(ComparisonOption value);

	const QDateTime& timeFilterValue() const;
	void setTimeFilterValue(const QDateTime& value);


	std::function<bool(const QFileInfo&)> createFilterFunction() const;
	std::function<bool(QStringView)> createMatchFunction() const;
	std::function<bool(QString&)> createReplaceFunction() const;

private:
	QString _path;
	QString _searchExpression;
	QString _replacementText;
	QStringList _wildcards;
	QStringList _excludes;
	bool _isCaseSensitive;
	SearchMode _searchMode;
	ComparisonOption _sizeFilterOption;
	qint64 _sizeFilterValue;
	ComparisonOption _timeFilterOption;
	QDateTime _timeFilterValue;
};
