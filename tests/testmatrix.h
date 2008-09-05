#ifndef TESTMATRIX_H
#define TESTMATRIX_H

#include <QtTest>

class TestMatrix : public QObject
{
	Q_OBJECT

private slots:
	void testConstructors();
	void testAssignment();
	void testOperators();
	void testSimpleOperators();
	void testTranspose();
	void testGl();
};

#endif
