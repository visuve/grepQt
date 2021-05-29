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
	};

	FileSearcher(QObject* parent, const QDir& directory, const QStringList& fileWildCards, const QRegularExpression& regex);
	FileSearcher(QObject* parent, const QDir& directory, const QStringList& fileWildCards, QStringView searchWord, Qt::CaseSensitivity caseSensitive);
	~FileSearcher();

signals:
	void processing(const QString& filePath);
	void matchFound(const Match& match);

private:
	void run() override;

	const QDir _directory;
	const QStringList _wildcards;
	std::function<bool(QStringView)> _matchFunction;
};
