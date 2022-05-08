#pragma once

class TestEncodingDetector: public QObject
{
	Q_OBJECT
private slots:
	void ansi();
	void utf8noBom();
	void utf8withBom();
	void utf16le();
	void utf16be();
};
