#include "fungespace.h"
#include "testfungespace.h"

QTEST_MAIN(TestFungeSpace);

void TestFungeSpace::initTestCase()
{
	m_space = new FungeSpace(3);
}

void TestFungeSpace::testStore()
{
	Coord coord(QVector<int>() << 1 << 2 << 3);
	
	m_space->setChar(coord, 'o');
	QCOMPARE(m_space->getChar(coord), QChar('o'));
}

void TestFungeSpace::testEmpty()
{
	Coord coord(QVector<int>() << 1 << 0 << 3);
	
	QCOMPARE(m_space->getChar(coord), QChar(' '));
}

void TestFungeSpace::cleanupTestCase()
{
	delete m_space;
}
