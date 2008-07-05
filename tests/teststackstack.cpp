#include "teststackstack.h"

#include "stackstack.h"
#include "interpreter.h"

QTEST_MAIN(TestStack);

void TestStack::init()
{
	m_model = new StackStackCollectionModel(NULL);
	m_ip = new Interpreter::InstructionPointer(m_model, Coord(), Coord(), Coord(), 0);
	m_stackstack = new StackStack(m_model, m_ip, NULL);
	m_stack = m_stackstack->topStack();
}

void TestStack::cleanup()
{
	delete m_stack;
	delete m_stackstack;
	delete m_ip;
	delete m_model;
}

void TestStack::testPush()
{
	QVERIFY(m_stack->count() == 0);
	m_stack->push(42);
	QVERIFY(m_stack->count() == 1);
	QVERIFY(m_stack->peek() == 42);
	QVERIFY(m_stack->count() == 1);
	QVERIFY(m_stack->pop() == 42);
	QVERIFY(m_stack->count() == 0);
}

void TestStack::testPushBottom()
{
	QVERIFY(m_stack->count() == 0);
	m_stack->push(1);
	m_stack->push(2);
	QVERIFY(m_stack->count() == 2);
	QVERIFY(m_stack->peek() == 2);
	m_stack->pushToBottom(3);
	QVERIFY(m_stack->count() == 3);
	QVERIFY(m_stack->peek() == 2);
	QVERIFY(m_stack->pop() == 2);
	QVERIFY(m_stack->pop() == 1);
	QVERIFY(m_stack->pop() == 3);
}

void TestStack::testClear()
{
	QVERIFY(m_stack->count() == 0);
	m_stack->push(42);
	QVERIFY(m_stack->count() == 1);
	m_stack->clear();
	QVERIFY(m_stack->count() == 0);
}

void TestStack::testStacks()
{
	QVERIFY(m_stackstack->topStack() == m_stack);
	QVERIFY(m_stackstack->secondStack() == NULL);
	QVERIFY(m_stackstack->count() == 1);
	QVERIFY(m_stackstack->ip() == m_ip);
	Stack* temp = m_stackstack->pushEmptyStack();
	QVERIFY(temp);
	QVERIFY(m_stackstack->count() == 2);
	QVERIFY(m_stackstack->topStack() == temp);
	QVERIFY(m_stackstack->secondStack() == m_stack);
	QVERIFY(temp->count() == 0);
}
