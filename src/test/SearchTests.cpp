#include "SearchTests.hpp"

void TestQString::toUpper()
{
	QString str = "Hello";
	QVERIFY(str.toUpper() == "HELLO");
}

QTEST_MAIN(TestQString)
#include "SearchTests.moc"
