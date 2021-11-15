#include "PCH.hpp"
#include "FileSearcher.hpp"
#include "Options.hpp"

FileSearcher::FileSearcher(Options* options, QObject* parent) :
	QThread(parent),
	_options(options)
{
}

FileSearcher::~FileSearcher()
{
	qDebug() << "Destroying...";
	requestInterruption();
	wait();
	qDebug() << "Destroyed.";
}

void FileSearcher::run()
{
	qDebug() << "Started";

	auto filterFunction = _options->createFilterFunction();
	auto breakFunction = _options->createBreakFunction();
	auto matchFunction = _options->createMatchFunction();

	std::array<char, 0x1000> buffer;
	QDirIterator iter(_options->path(), _options->wildcards(), QDir::Files, QDirIterator::Subdirectories);

	int filesProcessed = 0;
	int hits = 0;

	while (!QThread::currentThread()->isInterruptionRequested() && iter.hasNext())
	{
		const QString path = iter.next();

		if (!filterFunction(QFileInfo(path)))
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

			if (breakFunction(line))
			{
				qDebug() << "Skipped:" << path;
				break;
			}

			if (!matchFunction(line))
			{
				continue;
			}

			++hits;
			emit matchFound(path, lineNumber, line);
		}
	}

	emit searchCompleted(_options->path(), hits, filesProcessed);

	qDebug() << "Finished";
}
