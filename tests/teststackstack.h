#ifndef TESTSTACKSTACK_H
#define TESTSTACKSTACK_H

#include <QtTest>

#include "interpreter.h"

class Stack;
class StackStack;
class StackStackCollectionModel;

class TestStack : public QObject
{
	Q_OBJECT

private slots:
	void init();
	void cleanup();

	void testPush();
	void testPushBottom();
	void testClear();
	void testStacks();

private:
	StackStackCollectionModel* m_model;
	StackStack* m_stackstack;
	Stack* m_stack;
	Interpreter::InstructionPointer* m_ip;
};

#endif
