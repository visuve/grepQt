#include "FileSearcher.hpp"
#include "Options.hpp"
#include <uchardet.h>

const QRegularExpression lineSplit(QString::fromUtf16(u"\x2029|\\r\\n|\\r|\\n"));

class Uchardet
{
public:
	Uchardet() :
		_handle(uchardet_new())
	{
	}

	~Uchardet()
	{
		if (_handle)
		{
			uchardet_delete(_handle);
		}
	}

	int handleData(const char* data, size_t size) const
	{
		return uchardet_handle_data(_handle, data, size);
	}

	void dataEnd() const
	{
		uchardet_data_end(_handle);
	}

	bool sample(QByteArrayView data) const
	{
		int result = handleData(data.data(), data.size());
		dataEnd();
		return result == 0;
	}

	bool sample(QFile& file) const
	{
		QByteArray data = file.read(0x400);
		file.reset();

		if (data.isEmpty())
		{
			return false;
		}

		return sample(data);
	}

	QTextCodec* guessCodec() const
	{
		const char* charset = uchardet_get_charset(_handle);

		if (!charset || !charset[0])
		{
			return nullptr;
		}

		return QTextCodec::codecForName(charset);
	}

private:
	uchardet_t _handle;
};

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
	auto matchFunction = _options.createMatchFunction();

	QDirIterator iter(_options.path(), _options.wildcards(), QDir::Files, QDirIterator::Subdirectories);

	int filesProcessed = 0;
	QByteArray raw;
	QString decoded;
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

		Uchardet detector;
		detector.sample(file);

		QTextCodec* codec = detector.guessCodec();

		if (!codec)
		{
			qInfo() << "Cannot detect encoding for" << file.fileName();
			continue;
		}

		QTextDecoder* decoder = codec->makeDecoder(QStringConverter::Flag::ConvertInvalidToNull);

		while (!QThread::currentThread()->isInterruptionRequested() && !file.atEnd())
		{
			raw = file.read(0x400);
			decoded = decoder->toUnicode(raw);

			int lineNumber = 0;

			for (QString line : decoded.split(lineSplit))
			{
				++lineNumber;

				if (!matchFunction(line))
				{
					continue;
				}

				++hits;

				emit matchFound(path, lineNumber, line);
			}
		}

		delete decoder;
	}

	if (!QThread::currentThread()->isInterruptionRequested())
	{
		emit searchCompleted(_options.path(), hits, filesProcessed);
		qDebug() << "Finished";
	}
}
