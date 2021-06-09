#include "PCH.hpp"
#include "FileSearcher.hpp"

constexpr qint64 BufferSize = 0x1000; // 4kib

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

void FileSearcher::setReplaceFunction(std::function<void(QString&)> replaceFunction)
{
	_replaceFunction = replaceFunction;
}

void FileSearcher::run()
{
	qDebug() << "Started";

	std::array<char, BufferSize> buffer = {};
	QDirIterator iter(_directory, _wildcards, QDir::Files, QDirIterator::Subdirectories);

	int filesProcessed = 0;
	int hits = 0;
	qint64 currentPositionInStream = 0;
	qint64 rewindedPositionInStream = 0;

	while (!QThread::currentThread()->isInterruptionRequested() && iter.hasNext())
	{
		const QString path = iter.next();

		if (!_filterFunction(QFileInfo(path)))
		{
			qDebug() << "Filtered:" << path;
			continue;
		}

		QFile file(path);

		if (!file.open(QIODevice::ReadWrite | QIODevice::ExistingOnly))
		{
			qWarning() << "Could not open:" << path;
			continue;
		}

		emit processing(path, ++filesProcessed);

		for (int lineNumber = 1;
			!QThread::currentThread()->isInterruptionRequested() && !file.atEnd(); ++lineNumber)
		{
			qint64 lineSize = file.readLine(buffer.data(), BufferSize);
			QString line = QString::fromLocal8Bit(buffer.data(), lineSize);

			if (!_matchFunction(line))
			{
				continue;
			}

			if (_replaceFunction)
			{
				currentPositionInStream = file.pos();
				rewindedPositionInStream = currentPositionInStream - lineSize;

				if (!file.seek(rewindedPositionInStream))
				{
					qWarning() << "Failed to seek " << path <<
						"from" << currentPositionInStream << "to" << rewindedPositionInStream;
					break;
				}

				// TODO: shorter strings will fail

				_replaceFunction(line);
				QTextStream stream(&file);
				stream << line;
				stream.flush();
			}

			++hits;
			emit matchFound({ QDir::toNativeSeparators(path), lineNumber, line });
		}
	}

	emit searchCompleted(_directory, hits, filesProcessed);

	qDebug() << "Finished";
}
