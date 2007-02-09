#ifndef STACKSTACK_H
#define STACKSTACK_H

#include <Q3ListView>

#include "interpreter.h"

class StackStackCollectionModel : public Q3ListView
{
public:
	StackStackCollectionModel(QWidget* parent);
	
	StackStack* newStackStack(Interpreter::InstructionPointer* ip);
	StackStack* deepCopy(Interpreter::InstructionPointer* ip, StackStack* original);
};


class StackStack : public Q3ListViewItem
{
public:
	StackStack(Q3ListView* parent, Interpreter::InstructionPointer* ip, StackStack* original = NULL);
	
	QString text(int column) const;
	
	Stack* topStack();
	Stack* secondStack();
	
	Stack* pushEmptyStack();
	void removeTopStack();
	
	int count();
	Interpreter::InstructionPointer* ip() const { return m_ip; }
	
private:
	Interpreter::InstructionPointer* m_ip;
};


class Stack : public Q3ListViewItem
{
public:
	Stack(StackStack* parent);
	Stack(StackStack* parent, Stack* original);
	
	QString text(int column) const;
	
	int peek();
	int pop();
	void push(int value);
	void pushToBottom(int value);
	void clear();
	int count();
};


class DataCellItem : public Q3ListViewItem
{
public:
	DataCellItem(Stack* parent, int v);
	
	int value();
	void setValue(int v);
	
private:
	int m_value;
};

#endif
