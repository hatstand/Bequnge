#ifndef STACKSTACK_H
#define STACKSTACK_H

#include <QStack>
#include <QAbstractItemModel>

#include "interpreter.h"

class StackStack;
class Stack;
class DataItem;


class StackStackCollectionItem
{
public:
	enum Type
	{
		Type_DataItem,
		Type_Stack,
		Type_StackStack
	};
	
	virtual Type type() const = 0;
	virtual StackStackCollectionItem* parent() const = 0;
	virtual void* childInternalModelPointer(int row) const = 0;
	virtual int count() const = 0;
	virtual QVariant data(int row) const = 0;
};



class StackStackCollectionModel : public QAbstractItemModel
{
public:
	QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex& index) const;
	int rowCount(const QModelIndex& parent = QModelIndex()) const;
	int columnCount(const QModelIndex& parent = QModelIndex()) const;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	
	StackStack* newStackStack(Interpreter::InstructionPointer* ip);
	StackStack* deepCopy(StackStack* original);
	void removeStackStack(StackStack* stackStack);

private:
	QList<StackStack*> m_stackStacks;
};


class StackStack : public StackStackCollectionItem, QObject
{
	Q_OBJECT

public:
	StackStack(Interpreter::InstructionPointer* ip);
	
	Type type() { return Type_StackStack; }
	StackStackCollectionItem* parent() const { return NULL; }
	void* childInternalModelPointer(int row) { return m_stacks[row]; }
	int count() { return m_stacks.count(); }
	QVariant data(int row) { return "Stack stack"; }
	
	int indexOf(Stack* stack) { return m_stacks.indexOf(stack); }
	Interpreter::InstructionPointer* ip() { return m_ip; }
	
	void push(Stack* stack);
	Stack* pushEmptyStack();
	Stack* popStack();
	
	Stack* topStack();
	Stack* secondStack();

signals:
	void stackPushed(StackStack* stackStack);
	void stackPopped(StackStack* stackStack);

private:
	QList<Stack*> m_stacks;
	Interpreter::InstructionPointer* m_ip;
};


class Stack : public StackStackCollectionItem, QObject
{
	Q_OBJECT

public:
	Stack(StackStack* stackStack);
	
	Type type() const { return Type_Stack; }
	StackStackCollectionItem* parent() const { return m_stackStack; }
	void* childInternalModelPointer(int row) { return m_dataItem; }
	int count() { return m_stack.count(); }
	QVariant data(int row) { return "Stack"; }
	
	void push(const int c);
	int pop() const;
	int at(int i) const { return m_stack[i]; }

signals:
	void itemPushed(Stack* stack);
	void itemPopped(Stack* stack);

private:
	QStack<int> m_stack;
	StackStack* m_stackStack;
	DataItem* m_dataItem;
};


class DataItem : public StackStackCollectionItem
{
public:
	DataItem(Stack* parent) { m_parent = parent; }
	
	Type type() const { return Type_DataItem; }
	StackStackCollectionItem* parent() const { return m_parent; }
	void* childInternalModelPointer(int row) { return NULL; }
	int count() { return 0; }
	QVariant data(int row) { return m_parent->at(row); }

private:
	Stack* m_parent;
};

#endif
