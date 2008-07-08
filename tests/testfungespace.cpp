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
	Coord coord(QList<int>() << 1 << 2 << 3);
	
	m_space->setChar(coord, QChar('o').unicode());
	QCOMPARE(QChar(m_space->getChar(coord)), QChar('o'));
}

void TestFungeSpace::testEmpty()
{
	Coord coord(QList<int>() << 1 << 2 << 3);
	
	QCOMPARE(QChar(m_space->getChar(coord)), QChar(' '));
} 

void TestFungeSpace::testMultiStore()
{
	Coord coord(QList<int>() << 1 << 2 << 3);
	m_space->setChar(coord, 1);
	qDebug() << "Inserted 1";
	QCOMPARE(m_space->getChar(coord), 1);
	qDebug() << "Got 1";
	m_space->setChar(coord, 2);
	qDebug() << "Inserted 2";
	QCOMPARE(m_space->getChar(coord), 2);
}

void TestFungeSpace::insertValues()
{
	Coord front(QList<int>() << 0 << 0 << 0);
	Coord middle(QList<int>() << 0 << 0 << 1);
	Coord back(QList<int>() << 0 << 0 << 2);
	Coord front2(QList<int>() << 5 << 0 << 0);
	Coord middle2(QList<int>() << 5 << 0 << 1);
	Coord back2(QList<int>() << 5 << 0 << 2);

	Coord fronty(QList<int>() << 5 << 1 << 2);
	Coord frontx(QList<int>() << 4 << 1 << 2);

	// Make sure FrontComparison sorts correctly
	FrontComparison f;
	QVERIFY(f(front, middle));
	QVERIFY(f(middle, back));
	QVERIFY(f(front2, middle2));
	QVERIFY(f(middle2, back2));
	QVERIFY(f(frontx, fronty));
	QVERIFY(f(back, frontx));
	QVERIFY(f(back, fronty));
	QVERIFY(f(back2, frontx));
	QVERIFY(f(back2, fronty));
	QVERIFY(f(front, frontx));
	QVERIFY(f(front2, frontx));
	QVERIFY(f(middle, frontx));
	QVERIFY(f(middle2, frontx));

	
	// Insert the values checking the size increases each time
	QCOMPARE((ulong)m_space->m_space.size(), 0UL);
	m_space->setChar(middle, 'm');
	QVERIFY(m_space->codeByFront().begin()->coord == middle);
	QVERIFY(m_space->codeByFront().rbegin()->coord == middle);
	QCOMPARE((ulong)m_space->m_space.size(), 1UL);
	m_space->setChar(back, 'b');
	QVERIFY(m_space->codeByFront().begin()->coord == middle);
	QVERIFY(m_space->codeByFront().rbegin()->coord == back);
	QCOMPARE((ulong)m_space->m_space.size(), 2UL);
	m_space->setChar(front2, 'F');
	QVERIFY(m_space->codeByFront().begin()->coord == front2);
	QVERIFY(m_space->codeByFront().rbegin()->coord == back);
	QCOMPARE((ulong)m_space->m_space.size(), 3UL);
	m_space->setChar(front, 'f');
	QVERIFY(m_space->codeByFront().begin()->coord == front);
	QVERIFY(m_space->codeByFront().rbegin()->coord == back);
	QCOMPARE((ulong)m_space->m_space.size(), 4UL);
	m_space->setChar(middle2, 'M');
	QVERIFY(m_space->codeByFront().begin()->coord == front);
	QVERIFY(m_space->codeByFront().rbegin()->coord == back);
	QCOMPARE((ulong)m_space->m_space.size(), 5UL);
	m_space->setChar(back2, 'B');
	QVERIFY(m_space->codeByFront().begin()->coord == front);
	QVERIFY(m_space->codeByFront().rbegin()->coord == back2);
	QCOMPARE((ulong)m_space->m_space.size(), 6UL);
	m_space->setChar(frontx, 'X');
	QVERIFY(m_space->codeByFront().begin()->coord == front);
	QVERIFY(m_space->codeByFront().rbegin()->coord == frontx);
	QCOMPARE((ulong)m_space->m_space.size(), 7UL);
	m_space->setChar(fronty, 'Y');
	QVERIFY(m_space->codeByFront().begin()->coord == front);
	QVERIFY(m_space->codeByFront().rbegin()->coord == fronty);
	QCOMPARE((ulong)m_space->m_space.size(), 8UL);
	
	// Check the values are where we put them
	QCOMPARE(QChar(m_space->getChar(front)), QChar('f'));
	QCOMPARE(QChar(m_space->getChar(front2)), QChar('F'));
	QCOMPARE(QChar(m_space->getChar(middle)), QChar('m'));
	QCOMPARE(QChar(m_space->getChar(middle2)), QChar('M'));
	QCOMPARE(QChar(m_space->getChar(back)), QChar('b'));
	QCOMPARE(QChar(m_space->getChar(back2)), QChar('B'));
	QCOMPARE(QChar(m_space->getChar(frontx)), QChar('X'));
	QCOMPARE(QChar(m_space->getChar(fronty)), QChar('Y'));
}

void TestFungeSpace::testFrontSorting()
{
	insertValues();
	
	FungeSpace::CodeByFront& byFront(m_space->codeByFront());
	
	FungeSpace::CodeByFront::iterator it(byFront.begin());
	QCOMPARE(QChar(it->data).toLower(), QChar('f')); it++;
	QCOMPARE(QChar(it->data).toLower(), QChar('f')); it++;
	QCOMPARE(QChar(it->data).toLower(), QChar('m')); it++;
	QCOMPARE(QChar(it->data).toLower(), QChar('m')); it++;
	QCOMPARE(QChar(it->data).toLower(), QChar('b')); it++;
	QCOMPARE(QChar(it->data).toLower(), QChar('b')); it++;
	QCOMPARE(QChar(it->data).toLower(), QChar('x')); it++;
	QCOMPARE(QChar(it->data).toLower(), QChar('y')); it++;
	QCOMPARE(it, byFront.end());
}

void TestFungeSpace::testSideSorting()
{
	insertValues();
	
	FungeSpace::CodeBySide& bySide(m_space->codeBySide());
	
	FungeSpace::CodeBySide::iterator it(bySide.begin());
	QCOMPARE(QChar(it->data).isUpper(), true); it++;
	QCOMPARE(QChar(it->data).isUpper(), true); it++;
	QCOMPARE(QChar(it->data).isUpper(), true); it++;
	QCOMPARE(QChar(it->data).isUpper(), true); it++;
	QCOMPARE(QChar(it->data).isUpper(), true); it++;
	QCOMPARE(QChar(it->data).isLower(), true); it++;
	QCOMPARE(QChar(it->data).isLower(), true); it++;
	QCOMPARE(QChar(it->data).isLower(), true); it++;
	QCOMPARE(it, bySide.end());
}



