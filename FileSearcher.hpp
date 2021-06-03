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

	FileSearcher(
		QObject* parent,
		const QString& directory,
		const QStringList& fileWildCards,
		const std::function<bool(QStringView)> matchFunction,
		const std::function<bool(QFileInfo)> filterFunction);

	~FileSearcher();

signals:
	void processing(const QString& filePath, int filesProcessed);
	void matchFound(const Match& match);
	void searchCompleted(const QString& directory, int hits, int filesProcessed);

private:
	void run() override;

	const QString _directory;
	const QStringList _wildcards;
	std::function<bool(QStringView)> _matchFunction;
	std::function<bool(QFileInfo)> _filterFunction;
};
