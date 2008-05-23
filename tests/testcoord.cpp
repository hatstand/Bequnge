#include "testcoord.h"
#include "coord.h"

QTEST_MAIN(TestCoord);

void TestCoord::testOperators()
{
	Coord a(QList<int>() << 1 << 2 << 3);
	Coord b(QList<int>() << 1 << 2 << 3);
	
	QVERIFY(a == b);
	QVERIFY(!(a != b));
	
	Coord c(QList<int>() << 1 << 3 << 2);
	
	QVERIFY(a != c);
	QVERIFY(!(a == c));
	
	Coord d(QList<int>() << -1 << 3 << 2);
	
	QVERIFY(c != d);
	QVERIFY(!(c == d));
	
	
}

