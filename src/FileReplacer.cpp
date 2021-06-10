#include "PCH.hpp"
#include "FileReplacer.hpp"

constexpr qint64 BufferSize = 0x1000; // 4kib

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

void FileReplacer::setDirectory(const QString& directory)
{
	_directory = directory;
}

void FileReplacer::setWildcards(const QStringList& wildcards)
{
	_wildcards = wildcards;
}

void FileReplacer::setFilterFunction(std::function<bool(QFileInfo)> filterFunction)
{
	_filterFunction = filterFunction;
}

void FileReplacer::setReplaceFunction(std::function<void(QString&)> replaceFunction)
{
	_replaceFunction = replaceFunction;
}

void FileReplacer::run()
{
	qDebug() << "Started";

	qDebug() << "Finished";
}
