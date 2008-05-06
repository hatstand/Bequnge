#include <QtTest>

class FungeSpace;

class TestFungeSpace : public QObject
{
	Q_OBJECT

private slots:
	void initTestCase();
	
	void testStore();
	void testEmpty();
	
	void cleanupTestCase();

private:
	FungeSpace* m_space;
};
