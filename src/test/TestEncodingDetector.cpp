#include "TestEncodingDetector.hpp"

#include <icu.h>

#include "../core/EncodingDetector.hpp"

void TestEncodingDetector::ansi()
{
	constexpr char data[] = { 0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x21 };
	QPair<int, QString> result = EncodingDetector(data).encoding();
	QCOMPARE(result.first, 42);
	QVERIFY(result.second == "ISO-8859-1");
}

void TestEncodingDetector::utf8noBom()
{
	constexpr uint8_t data[] = { 0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x21, 0xF0, 0x9F, 0x99, 0x82 };
	QByteArray ba(reinterpret_cast<const char*>(data), sizeof(data));

	QPair<int, QString> result = EncodingDetector(ba).encoding();
	QCOMPARE(result.first, 80);
	QVERIFY(result.second == "UTF-8");
}

void TestEncodingDetector::utf8withBom()
{
	constexpr uint8_t data[] = { 0xEF, 0xBB, 0xBF, 0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x21, 0xF0, 0x9F, 0x99, 0x82 };
	QByteArray ba(reinterpret_cast<const char*>(data), sizeof(data));

	QPair<int, QString> result = EncodingDetector(ba).encoding();
	QCOMPARE(result.first, 100);
	QVERIFY(result.second == "UTF-8");
}

void TestEncodingDetector::utf16le()
{
	constexpr uint8_t data[] = { 0x48, 0x00, 0x65, 0x00, 0x6C, 0x00, 0x6C, 0x00, 0x6F, 0x00, 0x21, 0x00, 0x00, 0x3D, 0xD8, 0x42, 0xDE };
	QByteArray ba(reinterpret_cast<const char*>(data), sizeof(data));

	QPair<int, QString> result = EncodingDetector(ba).encoding();
	QCOMPARE(result.first, 70);
	QVERIFY(result.second == "UTF-16LE");
}

void TestEncodingDetector::utf16be()
{
	constexpr uint8_t data[] = { 0x00, 0x48, 0x00, 0x65, 0x00, 0x6C, 0x00, 0x6C, 0x00, 0x6F, 0x00, 0x21, 0x00, 0xD8, 0x3D, 0xDE, 0x42 };
	QByteArray ba(reinterpret_cast<const char*>(data), sizeof(data));

	QPair<int, QString> result = EncodingDetector(ba).encoding();
	QCOMPARE(result.first, 80);
	QVERIFY(result.second == "UTF-16BE");
}

QTEST_MAIN(TestEncodingDetector)
#include "TestEncodingDetector.moc"
