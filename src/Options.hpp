#pragma once

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
	void setPath(const QString&);

	const QStringList& wildcards() const;
	void setWildcards(const QStringList&);

	const QStringList& excludes() const;
	void setExcludes(const QStringList&);

	const QString& searchExpression() const;
	void setSearchExpression(const QString&);

	const QString& replacementText() const;
	void setReplacementText(const QString&);

	SearchMode searchMode() const;
	void setSearchMode(SearchMode);

	bool isCaseSensitive() const;
	void setCaseSensitive(bool);

	ComparisonOption sizeFilterOption() const;
	void setSizeFilterOption(ComparisonOption);

	qint64 sizeFilterFrom() const;
	void setSizeFilterFrom(qint64);

	qint64 sizeFilterTo() const;
	void setSizeFilterTo(qint64);

	ComparisonOption timeFilterOption() const;
	void setTimeFilterOption(ComparisonOption);

	const QDateTime& timeFilterFrom() const;
	void setTimeFilterFrom(const QDateTime&);

	const QDateTime& timeFilterTo() const;
	void setTimeFilterTo(const QDateTime&);

	std::function<bool(const QFileInfo&)> createFilterFunction() const;
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

	ComparisonOption _timeFilterOption;
	QDateTime _timeFilterFrom;
	QDateTime _timeFilterTo;
};
