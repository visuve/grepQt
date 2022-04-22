#include "FileSearcher.hpp"
#include "Options.hpp"
#include <icu.h>

const QRegularExpression lineSplit(QString::fromUtf16(u"\x2029|\\r\\n|\\r|\\n"));

class EncodingDetector
{
public:
	EncodingDetector() 
	{
		UErrorCode status = U_ZERO_ERROR;
		_detector = ucsdet_open(&status);
		Q_ASSERT(status == U_ZERO_ERROR);
	}

	~EncodingDetector()
	{
		if (_detector)
		{
			ucsdet_close(_detector);
		}
	}


	bool sample(QFile& file)
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
		if (_charset.isEmpty())
		{
			return nullptr;
		}

		return QTextCodec::codecForName(_charset);
	}
private:
	bool sample(QByteArrayView sample)
	{
		UErrorCode status = U_ZERO_ERROR;
		ucsdet_setText(_detector, sample.data(), sample.size(), &status);

		if (status != U_ZERO_ERROR)
		{
			return false;
		}

		const UCharsetMatch* match = ucsdet_detect(_detector, &status);

		if (status != U_ZERO_ERROR)
		{
			return false;
		}

		_charset = ucsdet_getName(match, &status);

		return status == U_ZERO_ERROR;
	}

	UCharsetDetector* _detector;
	QByteArray _charset;
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

		EncodingDetector detector;
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

			auto lines = decoded.split(lineSplit);
			int lineNumber = 0;

			// TODO: what if the last line does not produce a hit,
			// when in fact it's just truncated... :-(

			for (qsizetype i = 0; i < lines.size(); ++i)
			{
				++lineNumber;

				if (!matchFunction(lines[i]))
				{
					continue;
				}

				++hits;

				emit matchFound(path, lineNumber, lines[i]);
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
