#include "EncodingDetector.hpp"

EncodingDetector::EncodingDetector(QFile& file) :
	EncodingDetector()
{
	sample(file);
}

EncodingDetector::~EncodingDetector()
{
	if (_detector)
	{
		ucsdet_close(_detector);
	}
}

QTextCodec* EncodingDetector::codec() const
{
	if (_charset.isEmpty())
	{
		return nullptr;
	}

	return QTextCodec::codecForName(_charset);
}

EncodingDetector::EncodingDetector() :
	_detector(ucsdet_open(&_status))
{
	Q_ASSERT(_status == U_ZERO_ERROR);
}

bool EncodingDetector::sample(QFile& file)
{
	if (_status != U_ZERO_ERROR)
	{
		return false;
	}

	QByteArray data = file.read(0x400);
	file.reset();

	if (data.isEmpty())
	{
		return false;
	}

	return sample(data);
}

bool EncodingDetector::sample(QByteArrayView sample)
{
	if (_status != U_ZERO_ERROR)
	{
		return false;
	}

	ucsdet_setText(_detector, sample.data(), sample.size(), &_status);

	if (_status != U_ZERO_ERROR)
	{
		return false;
	}

	const UCharsetMatch* match = ucsdet_detect(_detector, &_status);

	if (_status != U_ZERO_ERROR)
	{
		return false;
	}

	_charset = ucsdet_getName(match, &_status);

	return _status == U_ZERO_ERROR;
}
