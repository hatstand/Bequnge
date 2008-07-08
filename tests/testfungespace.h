#ifndef TESTFUNGESPACE_H
#define TESTFUNGESPACE_H

#include <QtTest>

class FungeSpace;

class TestFungeSpace : public QObject
{
	Q_OBJECT

private slots:
	void init();
	void cleanup();
	
	// Basic tests
	void testStore();
	void testEmpty();
	void testMultiStore();
	
	// Sorting tests
	void testFrontSorting();
	void testSideSorting();

private:
	void insertValues();
	
	FungeSpace* m_space;
};

#endif
