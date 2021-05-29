#include "PCH.hpp"
#include "FileSearcher.hpp"

constexpr size_t BufferSize = 0x1000; // 4kib

FileSearcher::FileSearcher(
	QObject* parent,
	const QDir& directory,
	const QStringList& fileWildCards,
	const std::function<bool(QStringView)> matchFunction,
	const std::function<bool(QFileInfo)> filterFunction) :
	QThread(parent),
	_directory(directory),
	_wildcards(fileWildCards),
	_matchFunction(matchFunction),
	_filterFunction(filterFunction)
{
	connect(this, &QThread::terminate, []()
	{
		qDebug() << "Terminate requested";
	});

	connect(this, &QThread::quit, []()
	{
		qDebug() << "Quit requested";
	});
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
	std::array<wchar_t, BufferSize> buffer = {};
	QDirIterator iter(_directory.absolutePath(), _wildcards, QDir::Files, QDirIterator::Subdirectories);

	int filesProcessed = 0;

	while (iter.hasNext())
	{
		const QString path = iter.next();

		if (!_filterFunction(QFileInfo(path)))
		{
			continue;
		}

		std::wifstream stream(path.toStdWString());

		emit processing(path, ++filesProcessed);

		for (int lineNumber = 1; stream.getline(buffer.data(), BufferSize, '\n'); ++lineNumber)
		{
			const std::streamsize bytesRead = stream.gcount();

			if (bytesRead <= 0)
			{
				break;
			}

			const QStringView line(buffer.data(), bytesRead);

			if (_matchFunction(line))
			{
				emit matchFound({ path, lineNumber, line.toString() });
			}
		}
	}

	emit seachCompleted(filesProcessed);
}
