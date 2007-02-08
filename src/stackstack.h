#ifndef STACKSTACK_H
#define STACKSTACK_H

#include <QStack>
#include <QAbstractItemModel>

#include "interpreter.h"

class StackStack : public Q3ListViewItem
{
public:
	QString text(int column) const;
	
	Stack* topStack() const;
	Stack* secondStack() const;
	
private:
	Interpreter::InstructionPointer* m_ip;
	QStack<Stack*> m_stacks;
};

class Stack : public Q3ListViewItem
{
};

#endif
