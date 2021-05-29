#include "PCH.hpp"
#include "FileSearcher.hpp"

constexpr size_t BufferSize = 0x1000; // 4kib

FileSearcher::FileSearcher(QObject* parent, const QDir& directory, const QStringList& fileWildCards, const QRegularExpression& regex) :
	QThread(parent),
	_directory(directory),
	_wildcards(fileWildCards)
{
	connect(this, &QThread::terminate, []()
	{
		qDebug() << "Terminate requested";
	});

	connect(this, &QThread::quit, []()
	{
		qDebug() << "Quit requested";
	});

	_matchFunction = [=](QStringView haystack)
	{
		return regex.match(haystack).hasMatch();
	};
}

FileSearcher::FileSearcher(QObject* parent, const QDir& directory, const QStringList& fileWildCards, QStringView searchWord,  Qt::CaseSensitivity caseSensitive) :
	QThread(parent),
	_directory(directory),
	_wildcards(fileWildCards)
{
	connect(this, &QThread::terminate, []()
	{
		qDebug() << "Terminate requested";
	});

	connect(this, &QThread::quit, []()
	{
		qDebug() << "Quit requested";
	});

	_matchFunction = [=](QStringView haystack)
	{
		return haystack.contains(searchWord, caseSensitive);
	};
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
