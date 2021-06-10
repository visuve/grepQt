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
	void lineReplaced(const QString& filePath, int lineNumber, const QString& lineContent);
	void replaceCompleted(const QString& directory, int filesProcessed);

private:
	void run() override;
	const Options* _options;
};
