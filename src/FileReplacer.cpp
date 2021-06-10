#include "PCH.hpp"
#include "FileReplacer.hpp"

FileReplacer::FileReplacer(QObject* parent) :
	QThread(parent)
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

	std::array<char, 0x1000> buffer;
	QDirIterator iter(_directory, _wildcards, QDir::Files, QDirIterator::Subdirectories);

	int filesProcessed = 0;

	while (!QThread::currentThread()->isInterruptionRequested() && iter.hasNext())
	{
		const QString path = iter.next();

		if (!_filterFunction(QFileInfo(path)))
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

		QSaveFile outputFile(path);

		emit processing(path, ++filesProcessed);

		while (!QThread::currentThread()->isInterruptionRequested() && !inputFile.atEnd())
		{
			const qint64 lineSize = inputFile.readLine(buffer.data(), buffer.size());
			QString line = QString::fromLocal8Bit(buffer.data(), lineSize);
			_replaceFunction(line);
			outputFile.write(line.toLocal8Bit());
		}

		inputFile.close();
		outputFile.commit();
	}

	qDebug() << "Finished";
}
