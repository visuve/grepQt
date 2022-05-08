#pragma once

class EncodingDetector
{
public:
	EncodingDetector(QFile& file);
	EncodingDetector(QByteArrayView data);
	~EncodingDetector();

	QPair<int, QString> encoding() const;

private:
	EncodingDetector();
	void sample(QFile& file);
	void sample(QByteArrayView sample);

	UErrorCode _status = U_ZERO_ERROR;
	UCharsetDetector* _detector;
	QString _charset;
	int _confidence = 0;
};
