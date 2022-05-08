#include "TestEncodingDetector.hpp"
#include "TestMatchDetector.hpp"

int main(int argc, char** argv)
{
	int status = 0;
	status |= QTest::qExec(new TestEncodingDetector, argc, argv);
	status |= QTest::qExec(new TestMatchDetector, argc, argv);

	return status;
}
