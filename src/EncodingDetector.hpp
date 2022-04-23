#pragma once

class EncodingDetector
{
public:
	EncodingDetector(QFile& file);
	~EncodingDetector();

	QTextCodec* codec() const;

private:
	EncodingDetector();
	bool sample(QFile& file);
	bool sample(QByteArrayView sample);

	UErrorCode _status = U_ZERO_ERROR;
	UCharsetDetector* _detector;
	QByteArray _charset;
};
