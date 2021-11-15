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
		Between = 3
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

	SearchMode searchMode() const;
	void setSearchMode(SearchMode value);

	bool isCaseSensitive() const;
	void setCaseSensitive(bool value);

	ComparisonOption sizeFilterOption() const;
	void setSizeFilterOption(ComparisonOption value);

	qint64 sizeFilterFrom() const;
	void setSizeFilterFrom(qint64 value);

	qint64 sizeFilterTo() const;
	void setSizeFilterTo(qint64 value);

	bool skipBinary() const;
	void setSkipBinary(bool value);

	ComparisonOption timeFilterOption() const;
	void setTimeFilterOption(ComparisonOption value);

	const QDateTime& timeFilterFrom() const;
	void setTimeFilterFrom(const QDateTime& value);

	const QDateTime& timeFilterTo() const;
	void setTimeFilterTo(const QDateTime& value);

	std::function<bool(const QFileInfo&)> createFilterFunction() const;
	std::function<bool(QStringView)> createBreakFunction() const;
	std::function<bool(QStringView)> createMatchFunction() const;
	std::function<bool(QString&)> createReplaceFunction() const;

private:
	QString _path;
	QStringList _wildcards;
	QStringList _excludes;

	QString _searchExpression;
	QString _replacementText;
	SearchMode _searchMode;
	bool _isCaseSensitive;

	ComparisonOption _sizeFilterOption;
	qint64 _sizeFilterFrom;
	qint64 _sizeFilterTo;
	bool _skipBinary;

	ComparisonOption _timeFilterOption;
	QDateTime _timeFilterFrom;
	QDateTime _timeFilterTo;
};
