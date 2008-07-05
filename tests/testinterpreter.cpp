#include "testinterpreter.h"

#include "fungespace.h"
#include "interpreter.h"
#include "stackstack.h"

QTEST_MAIN(TestInterpreter);

void TestInterpreter::init()
{
	m_model = new StackStackCollectionModel(NULL);
	m_space = new FungeSpace(2);
	m_interpreter = new Interpreter(m_model, m_space);
}

void TestInterpreter::cleanup()
{
	delete m_model;
	delete m_space;
	delete m_interpreter;
}

void TestInterpreter::testPushItem()
{
	m_interpreter->pushItem(42);
	QVERIFY(m_interpreter->popItem());
}

void TestInterpreter::testAdd()
{
	QVERIFY(m_interpreter->ip()->m_pos == Coord());
	m_interpreter->ip()->stack()->push(1);
	m_interpreter->ip()->stack()->push(2);
	m_interpreter->add();
	QVERIFY(m_interpreter->ip()->stack()->count() == 1);
	QVERIFY(m_interpreter->ip()->stack()->peek() == 3);
}

void TestInterpreter::testSubtract()
{
	m_interpreter->ip()->stack()->push(2);
	m_interpreter->ip()->stack()->push(1);
	m_interpreter->subtract();
	QVERIFY(m_interpreter->ip()->stack()->count() == 1);
	QVERIFY(m_interpreter->ip()->stack()->peek() == 1);
}

void TestInterpreter::testMultiply()
{
	m_interpreter->ip()->stack()->push(2);
	m_interpreter->ip()->stack()->push(3);
	m_interpreter->multiply();
	QVERIFY(m_interpreter->ip()->stack()->count() == 1);
	QVERIFY(m_interpreter->ip()->stack()->peek() == 6);
}

void TestInterpreter::testDivide()
{
	m_interpreter->ip()->stack()->push(4);
	m_interpreter->ip()->stack()->push(2);
	m_interpreter->divide();
	QVERIFY(m_interpreter->ip()->stack()->count() == 1);
	QVERIFY(m_interpreter->ip()->stack()->peek() == 2);
}

void TestInterpreter::testModulo()
{
	m_interpreter->ip()->stack()->push(3);
	m_interpreter->ip()->stack()->push(2);
	m_interpreter->modulo();
	QVERIFY(m_interpreter->ip()->stack()->count() == 1);
	QVERIFY(m_interpreter->ip()->stack()->peek() == 1);
}

void TestInterpreter::testGreaterThan()
{
	m_interpreter->ip()->stack()->push(2);
	m_interpreter->ip()->stack()->push(1);
	m_interpreter->greaterThan();
	QVERIFY(m_interpreter->ip()->stack()->count() == 1);
	QVERIFY(m_interpreter->ip()->stack()->peek() == 1);
}

void TestInterpreter::testNotf()
{
	m_interpreter->ip()->stack()->push(1);
	m_interpreter->notf();
	QVERIFY(m_interpreter->ip()->stack()->count() == 1);
	QVERIFY(m_interpreter->ip()->stack()->peek() == 0);
}

void TestInterpreter::testUp()
{
	Coord expected;
	expected[0] = 0;
	expected[1] = -1;

	m_interpreter->up();
	QVERIFY(m_interpreter->ip()->m_direction == expected);
}

void TestInterpreter::testDown()
{
	Coord expected;
	expected[0] = 0;
	expected[1] = 1;

	m_interpreter->down();
	QVERIFY(m_interpreter->ip()->m_direction == expected);
}

void TestInterpreter::testLeft()
{
	Coord expected;
	expected[0] = -1;
	expected[1] = 0;

	m_interpreter->left();
	QVERIFY(m_interpreter->ip()->m_direction == expected);
}

void TestInterpreter::testRight()
{
	Coord expected;
	expected[0] = 1;
	expected[1] = 0;

	m_interpreter->right();
	QVERIFY(m_interpreter->ip()->m_direction == expected);
}
