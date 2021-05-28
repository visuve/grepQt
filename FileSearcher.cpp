#include "FileSearcher.hpp"
#include <QDebug>
#include <QDirIterator>

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
		return regex.match(haystack).hasMatch();
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

}
