#pragma once

class Options;

class FileReplacer: public QThread
{
	Q_OBJECT

public:
	FileReplacer(Options* options, QObject* parent);
	~FileReplacer();

signals:
	void processing(const QString& filePath, int filesProcessed);
	void fileUpdated(const QString& filePath);
	void replacementCompleted(const QString& directory, int hits, int filesProcessed);

private:
	void run() override;
	const Options* _options;
};
