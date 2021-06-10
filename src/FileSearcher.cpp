#include "PCH.hpp"
#include "FileSearcher.hpp"

FileSearcher::FileSearcher(QObject* parent) :
	QThread(parent)
{
}

FileSearcher::~FileSearcher()
{
	qDebug() << "Destroying...";
	requestInterruption();
	wait();
	qDebug() << "Destroyed.";
}

void FileSearcher::setDirectory(const QString& directory)
{
	_directory = directory;
}

void FileSearcher::setWildcards(const QStringList& wildcards)
{
	_wildcards = wildcards;
}

void FileSearcher::setMatchFunction(std::function<bool (QStringView)> matchFunction)
{
	_matchFunction = matchFunction;
}

void FileSearcher::setFilterFunction(std::function<bool (QFileInfo)> filterFunction)
{
	_filterFunction = filterFunction;
}

void FileSearcher::run()
{
	qDebug() << "Started";

	std::array<char, 0x1000> buffer;
	QDirIterator iter(_directory, _wildcards, QDir::Files, QDirIterator::Subdirectories);

	int filesProcessed = 0;
	int hits = 0;

	while (!QThread::currentThread()->isInterruptionRequested() && iter.hasNext())
	{
		const QString path = iter.next();

		if (!_filterFunction(QFileInfo(path)))
		{
			qDebug() << "Filtered:" << path;
			continue;
		}

		QFile file(path);

		if (!file.open(QIODevice::ReadOnly))
		{
			qWarning() << "Could not open:" << path;
			continue;
		}

		emit processing(path, ++filesProcessed);

		for (int lineNumber = 1;
			!QThread::currentThread()->isInterruptionRequested() && !file.atEnd(); ++lineNumber)
		{
			qint64 lineSize = file.readLine(buffer.data(), buffer.size());
			QString line = QString::fromLocal8Bit(buffer.data(), lineSize);

			if (!_matchFunction(line))
			{
				continue;
			}

			++hits;
			emit matchFound({ QDir::toNativeSeparators(path), lineNumber, line });
		}
	}

	emit searchCompleted(_directory, hits, filesProcessed);

	qDebug() << "Finished";
}
