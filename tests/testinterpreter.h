#ifndef TESTINTERPRETER_H
#define TESTINTERPRETER_H

#include <QObject>
#include <QtTest>

class StackStackCollectionModel;
class FungeSpace;
class Interpreter;

class TestInterpreter : public QObject
{
	Q_OBJECT
private slots:
	void init();
	void cleanup();

	void testPushItem();
	void testAdd();
	void testSubtract();
	void testMultiply();
	void testDivide();
	void testModulo();
	void testGreaterThan();
	void testNotf();
	void testUp();
	void testDown();
	void testLeft();
	void testRight();
	void testTurnLeft();
	void testTurnRight();
	void testHigher();
	void testLower();
	void testSimpleBeginBlock();
	void testCopyItemsBeginBlock();
	void testEmptyBlockDoesNothing();
	void testCopyItemsEndBlock();
	void testCopyZeroesBeginBlock();
	void testCopyTooManyItemsBeginBlock();

private:
	StackStackCollectionModel* m_model;
	FungeSpace* m_space;
	Interpreter* m_interpreter;
};

#endif
