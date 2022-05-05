#pragma once

class Options;

class FileSearcher : public QThread
{
	Q_OBJECT

public:
	FileSearcher(const Options& options, QObject* parent);
	~FileSearcher();

signals:
	void processing(const QString& filePath, int filesProcessed);
	void matchFound(const QString& filePath, int lineNumber, const QString& lineContent);
	void completed(const QString& directory, int filesProcessed);

private:
	void run() override;
	const Options& _options;
};
