#include "FileSearcher.hpp"
#include <QDebug>
#include <QDirIterator>

#include <filesystem>
#include <fstream>

constexpr size_t BufferSize = 0x1000; // 4kib

FileSearcher::FileSearcher(QObject* parent, const QDir& directory, const QRegularExpression& regex) :
	QThread(parent),
	_directory(directory)
{
	connect(this, &QThread::terminate, []()
	{
		qDebug() << "Terminate requested";
	});

	connect(this, &QThread::quit, []()
	{
		qDebug() << "Quit requested";
	});

	_matchFunction = [&](QStringView haystack)
	{
		return haystack.contains(regex);
	};
}

FileSearcher::FileSearcher(QObject* parent, const QDir& directory, QStringView searchWord,  Qt::CaseSensitivity caseSensitive) :
	QThread(parent),
	_directory(directory)
{
	connect(this, &QThread::terminate, []()
	{
		qDebug() << "Terminate requested";
	});

	connect(this, &QThread::quit, []()
	{
		qDebug() << "Quit requested";
	});

	_matchFunction = [&](QStringView haystack)
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
	QDirIterator iter(_directory.absolutePath(), { "*.txt" }, QDir::Files, QDirIterator::Subdirectories);

	while (iter.hasNext())
	{
		searchFile(iter.next());
	}
}

void FileSearcher::searchFile(QStringView path)
{
	thread_local std::array<QString::value_type, BufferSize> buffer = {};

	std::filesystem::path p(path.cbegin(), path.cend());
	std::basic_ifstream<QString::value_type> stream(p);

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
			emit matchFound({ path.toString(), lineNumber, line.toString() });
		}
	}
}
