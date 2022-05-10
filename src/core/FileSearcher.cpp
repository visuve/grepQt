#include "FileSearcher.hpp"
#include "Options.hpp"
#include "EncodingDetector.hpp"
#include "MatchDetector.hpp"

FileSearcher::FileSearcher(const Options& options, QObject* parent) :
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

	auto filterFunction = _options.createFilterFunction();

	const QStringList& wildcards = _options.wildcards();

	std::unique_ptr<QDirIterator> iter = wildcards.isEmpty() ?
		std::make_unique<QDirIterator>(_options.path(), QDir::Files, QDirIterator::Subdirectories):
		std::make_unique<QDirIterator>(_options.path(), _options.wildcards(), QDir::Files, QDirIterator::Subdirectories);

	int filesProcessed = 0;
	std::array<char, 0x2000> raw;

	QString decoded;

	while (!QThread::currentThread()->isInterruptionRequested() && iter->hasNext())
	{
		const QString path = iter->next();

		if (!filterFunction(iter->fileInfo()))
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

		EncodingDetector detector(file);

		const QPair<int, QString> encoding = detector.encoding();

		// Buggy ICU...
		if (encoding.first <= 10 || (encoding.first <= 30 && encoding.second == "UTF-16BE"))
		{
			qInfo() << "Cannot detect encoding for" << file.fileName();
			continue;
		}

		qDebug() << file.fileName() << "appears" << encoding;

		MatchDetector matcher(_options, encoding.second);

		while (!QThread::currentThread()->isInterruptionRequested() && !file.atEnd())
		{
			const qint64 bytesRead = file.read(raw.data(), raw.size());

			if (!matcher.feed({ raw.data(), bytesRead }, file.atEnd()))
			{
				qInfo() << "Cannot process" << path;
				break;
			}
		}

		/*for (const auto& [line, content] : matcher)
		{
			emit matchFound(path, line, QString::fromStdU16String(content));
		}*/
	}

	if (!QThread::currentThread()->isInterruptionRequested())
	{
		emit completed(_options.path(), filesProcessed);
		qDebug() << "Finished";
	}
}
