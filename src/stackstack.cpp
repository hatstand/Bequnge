#include "stackstack.h"

#include <QChar>

StackStackCollectionModel::StackStackCollectionModel(QWidget* parent)
	: Q3ListView(parent)
{
}

StackStack* StackStackCollectionModel::newStackStack(Interpreter::InstructionPointer* ip)
{
	return new StackStack(this, ip);
}

StackStack* StackStackCollectionModel::deepCopy(Interpreter::InstructionPointer* ip, StackStack* original)
{
	return new StackStack(this, ip, original);
}

StackStack::StackStack(Q3ListView* parent, Interpreter::InstructionPointer* ip)
	: Q3ListViewItem(parent), m_ip(ip)
{
	pushEmptyStack();
	setOpen(true);
}

StackStack::StackStack(Q3ListView* parent, Interpreter::InstructionPointer* ip, StackStack* original)
	: Q3ListViewItem(parent), m_ip(ip)
{
	Stack* stack = (Stack*) original->firstChild();
	
	QStack<Stack*> tempStack;
	while (stack)
	{
		tempStack.push(stack);
		stack = (Stack*) stack->nextSibling();
	}
	while (tempStack.count() > 0)
		new Stack(this, tempStack.pop());
	
	setOpen(true);
}

QString StackStack::text(int column) const
{
	return "Instruction pointer " + QString::number(m_ip->m_id);
}

Stack* StackStack::topStack()
{
	if (childCount() < 1)
		return pushEmptyStack();
	return (Stack*) firstChild();
}

Stack* StackStack::secondStack()
{
	if (childCount() < 1)
		pushEmptyStack();
	return (Stack*) firstChild()->nextSibling();
}

Stack* StackStack::pushEmptyStack()
{
	return new Stack(this);
}

void StackStack::removeTopStack()
{
	delete firstChild();
}

int StackStack::count()
{
	return childCount();
}

Stack::Stack(StackStack* parent)
	: Q3ListViewItem(parent)
{
	setOpen(true);
}

Stack::Stack(StackStack* parent, Stack* original)
	: Q3ListViewItem(parent)
{
	DataCellItem* item = (DataCellItem*) original->firstChild();
	
	QStack<int> tempStack;
	while (item)
	{
		tempStack.push(item->value());
		item = (DataCellItem*) item->nextSibling();
	}
	while (tempStack.count() > 0)
		push(tempStack.pop());
	
	setOpen(true);
}

QString Stack::text(int column) const
{
	return "Stack";
}

int Stack::peek()
{
	if (childCount() == 0)
		return 0;
	
	return ((DataCellItem*) firstChild())->value();
}

int Stack::pop()
{
	if (childCount() == 0)
		return 0;
	
	int ret = ((DataCellItem*) firstChild())->value();
	delete firstChild();
	return ret;
}

void Stack::push(int value)
{
	new DataCellItem(this, value);
}

void Stack::pushToBottom(int value)
{
	DataCellItem* newItem = new DataCellItem(this, value);
	
	if (childCount() == 0)
		return;
	
	// Find the last child
	DataCellItem* lastChild = (DataCellItem*) firstChild();
	while (lastChild->nextSibling() != NULL)
		lastChild = (DataCellItem*) lastChild->nextSibling();
	newItem->moveItem(lastChild);
}

void Stack::clear()
{
	while (firstChild())
		delete firstChild();
}

int Stack::count()
{
	return childCount();
}

DataCellItem::DataCellItem(Stack* parent, int v)
	: Q3ListViewItem(parent)
{
	setValue(v);
}

int DataCellItem::value()
{
	return m_value;
}

void DataCellItem::setValue(int v)
{
	m_value = v;
	setText(0, QString::number(v) + " (" + QChar(v) + ")");
}

