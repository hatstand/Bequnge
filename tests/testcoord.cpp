#include "testcoord.h"
#include "coord.h"

QTEST_MAIN(TestCoord);

void TestCoord::testOperators()
{
	Coord a(QVector<int>() << 1 << 2 << 3);
	Coord b(QVector<int>() << 1 << 2 << 3);
	
	QVERIFY(a == b);
	QVERIFY(!(a != b));
	
	Coord c(QVector<int>() << 1 << 3 << 2);
	
	QVERIFY(a != c);
	QVERIFY(!(a == c));
	
	Coord d(QVector<int>() << -1 << 3 << 2);
	
	QVERIFY(c != d);
	QVERIFY(!(c == d));
	
	
}

