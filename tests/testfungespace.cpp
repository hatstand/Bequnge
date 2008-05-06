//#define BOOST_MULTI_INDEX_ENABLE_SAFE_MODE
//#define BOOST_MULTI_INDEX_ENABLE_INVARIANT_CHECKING

#include "testfungespace.h"
#include "fungespace.h"

QTEST_MAIN(TestFungeSpace);

void TestFungeSpace::init()
{
	m_space = new FungeSpace(3);
}

void TestFungeSpace::cleanup()
{
	delete m_space;
}

void TestFungeSpace::testStore()
{
	Coord coord(QVector<int>() << 1 << 2 << 3);
	
	m_space->setChar(coord, 'o');
	QCOMPARE(m_space->getChar(coord), QChar('o'));
}

void TestFungeSpace::testEmpty()
{
	Coord coord(QVector<int>() << 1 << 2 << 3);
	
	QCOMPARE(m_space->getChar(coord), QChar(' '));
}

void TestFungeSpace::insertValues()
{
	Coord front(QVector<int>() << 0 << 0 << 0);
	Coord middle(QVector<int>() << 0 << 0 << 1);
	Coord back(QVector<int>() << 0 << 0 << 2);
	Coord front2(QVector<int>() << -2 << 2 << 0);
	Coord middle2(QVector<int>() << 2 << 2 << 1);
	Coord back2(QVector<int>() << 0 << 3 << 2);
	
	QCOMPARE(m_space->m_space.size(), (uint)0);
	m_space->setChar(middle, 'm');
	QCOMPARE(m_space->m_space.size(), (uint)1);
	m_space->setChar(back, 'b');
	QCOMPARE(m_space->m_space.size(), (uint)2);
	m_space->setChar(front2, 'f');
	QCOMPARE(m_space->m_space.size(), (uint)3);
	m_space->setChar(front, 'f');
	QCOMPARE(m_space->m_space.size(), (uint)4);
	m_space->setChar(middle2, 'm');
	QCOMPARE(m_space->m_space.size(), (uint)5);
	m_space->setChar(back2, 'b');
	QCOMPARE(m_space->m_space.size(), (uint)6);
	
	QCOMPARE(m_space->getChar(front), QChar('f'));
	QCOMPARE(m_space->getChar(front2), QChar('f'));
	QCOMPARE(m_space->getChar(middle), QChar('m'));
	QCOMPARE(m_space->getChar(middle2), QChar('m'));
	QCOMPARE(m_space->getChar(back), QChar('b'));
	QCOMPARE(m_space->getChar(back2), QChar('b'));
}

void TestFungeSpace::testFrontSorting()
{
	insertValues();
	
	FungeSpace::CodeByFront& byFront(m_space->codeByFront());
	
	FungeSpace::CodeByFront::iterator it(byFront.begin());
	QCOMPARE(it->data, QChar('f')); it++;
	QCOMPARE(it->data, QChar('f')); it++;
	QCOMPARE(it->data, QChar('m')); it++;
	QCOMPARE(it->data, QChar('m')); it++;
	QCOMPARE(it->data, QChar('b')); it++;
	QCOMPARE(it->data, QChar('b')); it++;
}

void TestFungeSpace::testSideSorting()
{
}
