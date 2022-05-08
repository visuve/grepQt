#include "EncodingDetector.hpp"

EncodingDetector::EncodingDetector(QFile& file) :
	EncodingDetector()
{
	sample(file);
}

EncodingDetector::EncodingDetector(QByteArrayView data) :
	EncodingDetector()
{
	sample(data);
}

EncodingDetector::~EncodingDetector()
{
	if (_detector)
	{
		ucsdet_close(_detector);
	}
}

QPair<int, QString> EncodingDetector::encoding() const
{
	return { U_SUCCESS(_status) ? _confidence : 0, _charset };
}

EncodingDetector::EncodingDetector() :
	_detector(ucsdet_open(&_status))
{
	Q_ASSERT(U_SUCCESS(_status));
}

void EncodingDetector::sample(QFile& file)
{
	QByteArray data = file.read(0x400);
	file.reset();

	if (data.isEmpty())
	{
		return;
	}

	return sample(data);
}

void EncodingDetector::sample(QByteArrayView sample)
{
	ucsdet_setText(_detector, sample.data(), sample.size(), &_status);

	if (U_FAILURE(_status))
	{
		return;
	}

	const UCharsetMatch* match = ucsdet_detect(_detector, &_status);

	if (U_FAILURE(_status))
	{
		return;
	}

	_confidence = ucsdet_getConfidence(match, &_status);

	if (U_FAILURE(_status))
	{
		return;
	}

	_charset = ucsdet_getName(match, &_status);
}
