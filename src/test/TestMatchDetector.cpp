#include "TestMatchDetector.hpp"

#include <icu.h>

#include "MatchDetector.hpp"

TestMatchDetector::TestMatchDetector() :
	_options("TestFileSearcher")
{
	_options.setCaseSensitive(false);
	_options.setSearchExpression("IHANOITA\r\nSILAKOITA");
	_options.setSearchMode(Options::SearchMode::Plain);
}

void TestMatchDetector::find_data()
{
	QTest::addColumn<QString>("resourceKey");
	QTest::addColumn<QString>("encoding");

	QTest::newRow("ANSI") << ":/data/ANSI.txt" << "ISO-8859-1";
	QTest::newRow("UTF-8") << ":/data/UTF-8.txt" << "UTF-8";
	QTest::newRow("UTF-8 BOM") << ":/data/UTF-8-BOM.txt" << "UTF-8";

	QTest::newRow("UTF-16 LE") << ":/data/UTF-16LE.txt" << "UTF-16LE";
	QTest::newRow("UTF-16 LE BOM") << ":/data/UTF-16LE-BOM.txt" << "UTF-16LE";

	QTest::newRow("UTF-16 BE") << ":/data/UTF-16BE.txt" << "UTF-16BE";
	QTest::newRow("UTF-16 BE BOM") << ":/data/UTF-16BE-BOM.txt" << "UTF-16BE";
}

void TestMatchDetector::find()
{
	QFETCH(QString, resourceKey);
	QFETCH(QString, encoding);

	MatchDetector md(_options, encoding);

	QFile file(resourceKey);
	file.open(QFile::ReadOnly);

	while (!file.atEnd())
	{
		QByteArray data = file.read(6);
		QVERIFY(md.feed(data, file.atEnd()));
	}

	auto results = md.matches();

	QVERIFY(results.size() == 4);
}

#include "TestMatchDetector.moc"
