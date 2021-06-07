#include "PCH.hpp"
#include "FileSearcher.hpp"

constexpr size_t BufferSize = 0x1000; // 4kib

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
	std::array<wchar_t, BufferSize> buffer = {};
	QDirIterator iter(_directory, _wildcards, QDir::Files, QDirIterator::Subdirectories);

	int filesProcessed = 0;
	int hits = 0;

	while (iter.hasNext() && QThread::currentThread()->isInterruptionRequested() == false)
	{
		const QString path = iter.next();

		if (!_filterFunction(QFileInfo(path)))
		{
			continue;
		}

		std::wifstream stream(path.toStdWString());

		emit processing(path, ++filesProcessed);

		for (int lineNumber = 1;
			stream.getline(buffer.data(), BufferSize, '\n') &&
			QThread::currentThread()->isInterruptionRequested() == false;
			++lineNumber)
		{
			const std::streamsize bytesRead = stream.gcount();

			if (bytesRead <= 0)
			{
				break;
			}

			const QStringView line(buffer.data(), bytesRead);

			if (_matchFunction(line))
			{
				++hits;
				emit matchFound({ QDir::toNativeSeparators(path), lineNumber, line.toString() });
			}
		}
	}

	emit searchCompleted(_directory, hits, filesProcessed);
}
