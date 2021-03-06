#include "FileReplacer.hpp"
#include "EncodingDetector.hpp"
#include "Options.hpp"

FileReplacer::FileReplacer(const Options& options, QObject* parent) :
	QThread(parent),
	_options(options)
{
}

FileReplacer::~FileReplacer()
{
	qDebug() << "Destroying...";
	requestInterruption();
	wait();
	qDebug() << "Destroyed.";
}

void FileReplacer::run()
{
	qDebug() << "Started";

	auto filterFunction = _options.createFilterFunction();
	auto replaceFunction = _options.createReplaceFunction();

	std::array<char, 0x1000> buffer;
	QDirIterator iter(_options.path(), _options.wildcards(), QDir::Files, QDirIterator::Subdirectories);

	int filesProcessed = 0;

	while (!QThread::currentThread()->isInterruptionRequested() && iter.hasNext())
	{
		const QString path = iter.next();

		if (!filterFunction(iter.fileInfo()))
		{
			qDebug() << "Filtered:" << path;
			continue;
		}

		QFile inputFile(path);

		if (!inputFile.open(QIODevice::ReadWrite | QIODevice::ExistingOnly))
		{
			qWarning() << "Could not open:" << path;
			continue;
		}

		EncodingDetector detector(inputFile);

		const QPair<int, QString> encoding = detector.encoding();

		// Buggy ICU...
		if (encoding.first <= 10 || (encoding.first <= 30 && encoding.second == "UTF-16BE"))
		{
			qInfo() << "Cannot detect encoding for" << inputFile.fileName();
			continue;
		}

		qDebug() << inputFile.fileName() << "appears" << encoding;

		QSaveFile outputFile(path);

		if (!outputFile.open(QIODevice::WriteOnly))
		{
			qWarning() << "Could not open temporary file";
			continue;
		}

		emit processing(path, ++filesProcessed);

		for (int lineNumber = 1;
			!QThread::currentThread()->isInterruptionRequested() && !inputFile.atEnd(); ++lineNumber)
		{
			const qint64 lineSize = inputFile.readLine(buffer.data(), buffer.size());
			QString line = QString::fromLocal8Bit(buffer.data(), lineSize);

			if (replaceFunction(line))
			{
				emit lineReplaced(path, lineNumber, line);
			}

			outputFile.write(line.toLocal8Bit());
		}

		inputFile.close();
		outputFile.commit();
	}

	if (!QThread::currentThread()->isInterruptionRequested())
	{
		emit replaceCompleted(_options.path(), filesProcessed);
		qDebug() << "Finished";
	}
}
