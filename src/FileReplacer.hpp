#pragma once

class FileReplacer: public QThread
{
	Q_OBJECT

public:
	FileReplacer(QObject* parent);
	~FileReplacer();

signals:
	void processing(const QString& filePath, int filesProcessed);
	void fileUpdated(const QString& filePath);
	void replacementCompleted(const QString& directory, int hits, int filesProcessed);

private:
	void run() override;

	QString _directory;
	QStringList _wildcards;
	std::function<bool(QFileInfo)> _filterFunction;
	std::function<void(QString&)> _replaceFunction;
};
