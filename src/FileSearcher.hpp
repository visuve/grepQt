#pragma once

class FileSearcher : public QThread
{
	Q_OBJECT

public:
	struct Match
	{
		QString filePath;
		int lineNumber;
		QString lineContent;

		inline QString toString() const
		{
			return QString("%1:%2:%3").arg(filePath).arg(lineNumber).arg(lineContent);
		}
	};

	FileSearcher(QObject* parent);
	~FileSearcher();

signals:
	void processing(const QString& filePath, int filesProcessed);
	void matchFound(const Match& match);
	void searchCompleted(const QString& directory, int hits, int filesProcessed);

private:
	void run() override;

	QString _directory;
	QStringList _wildcards;
	std::function<bool(QFileInfo)> _filterFunction;
	std::function<bool(QStringView)> _matchFunction;
};
