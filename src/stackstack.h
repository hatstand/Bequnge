#ifndef STACKSTACK_H
#define STACKSTACK_H

#include <QTreeWidget>
#include <QTreeWidgetItem>

#include "interpreter.h"

class StackStackCollectionModel : public QTreeWidget
{
public:
	StackStackCollectionModel(QWidget* parent);
	
	StackStack* newStackStack(Interpreter::InstructionPointer* ip);
	StackStack* deepCopy(Interpreter::InstructionPointer* ip, StackStack* original);
};


class StackStack : public QTreeWidgetItem
{
public:
	StackStack(StackStackCollectionModel* parent, Interpreter::InstructionPointer* ip, StackStack* original = NULL);
	
	Stack* topStack();
	Stack* secondStack();
	
	Stack* pushEmptyStack();
	void removeTopStack();
	
	int count();
	Interpreter::InstructionPointer* ip() const { return m_ip; }
	
private:
	Interpreter::InstructionPointer* m_ip;
};


class Stack : public QTreeWidgetItem
{
public:
	Stack(StackStack* parent, Stack* original = NULL);
	
	int peek();
	int pop();
	void push(int value);
	void pushToBottom(int value);
	void clear();
	int count();
	void print();
};


class DataCellItem : public QTreeWidgetItem
{
public:
	DataCellItem(Stack* parent, int v);
	
	int value();
	void setValue(int v);
	
private:
	int m_value;
};

#endif
