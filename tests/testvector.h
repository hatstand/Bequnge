#ifndef TESTVECTOR_H
#define TESTVECTOR_H

#include <QtTest>

class TestVector : public QObject
{
	Q_OBJECT

private slots:
	void testOperators();
};

#endif
