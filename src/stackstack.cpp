#include "stackstack.h"

#include <QChar>

StackStackCollectionModel::StackStackCollectionModel(QWidget* parent)
	: QTreeWidget(parent)
{
	setHeaderLabel("Data");
	setSortingEnabled(false);
}

StackStack* StackStackCollectionModel::newStackStack(Interpreter::InstructionPointer* ip)
{
	return new StackStack(this, ip);
}

StackStack* StackStackCollectionModel::deepCopy(Interpreter::InstructionPointer* ip, StackStack* original)
{
	return new StackStack(this, ip, original);
}

StackStack::StackStack(StackStackCollectionModel* parent, Interpreter::InstructionPointer* ip, StackStack* original)
	: QTreeWidgetItem(0), m_ip(ip)
{
	parent->insertTopLevelItem(0, this);
	setExpanded(true);
	
	if (original == NULL)
		pushEmptyStack();
	else
	{
		for (int i=original->childCount()-1 ; i>=0 ; --i)
			new Stack(this, (Stack*) original->child(i));
	}
	
	QPixmap icon(16, 16);
	icon.fill(ip->m_color);
	setIcon(0, QIcon(icon));
	
	setText(0, "Instruction pointer " + QString::number(m_ip->m_id));
}

Stack* StackStack::topStack()
{
	if (childCount() < 1)
		return pushEmptyStack();
	return (Stack*) child(0);
}

Stack* StackStack::secondStack()
{
	if (childCount() < 1)
		pushEmptyStack();
	return (Stack*) child(1);
}

Stack* StackStack::pushEmptyStack()
{
	return new Stack(this);
}

void StackStack::removeTopStack()
{
	delete child(0);
}

int StackStack::count()
{
	return childCount();
}

Stack::Stack(StackStack* parent, Stack* original)
	: QTreeWidgetItem(0)
{
	parent->insertChild(0, this);
	setExpanded(true);
	
	if (original != NULL)
	{
		for (int i=original->childCount()-1 ; i>=0 ; --i)
			new DataCellItem(this, ((DataCellItem*) original->child(i))->value());
	}
	
	setText(0, "Stack");
}

int Stack::peek()
{
	if (childCount() == 0)
		return 0;
	
	return ((DataCellItem*) child(0))->value();
}

int Stack::pop()
{
	if (childCount() == 0)
		return 0;
	
	int ret = ((DataCellItem*) child(0))->value();
	delete child(0);
	return ret;
}

void Stack::push(int value)
{
	new DataCellItem(this, value);
}

void Stack::pushToBottom(int value)
{
	DataCellItem* newItem = new DataCellItem(NULL, value);
	insertChild(childCount(), newItem);
}

void Stack::clear()
{
	while (childCount() > 0)
		delete child(childCount()-1);
}

int Stack::count()
{
	return childCount();
}

void Stack::print()
{
	for (int i = 0; i < childCount(); ++i)
	{
		qDebug() << ((DataCellItem*)child(i))->value();
	}
}

DataCellItem::DataCellItem(Stack* parent, int v)
	: QTreeWidgetItem(0)
{
	setValue(v);
	if (parent)
		parent->insertChild(0, this);
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

