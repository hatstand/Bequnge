#include "stackstack.h"

#include <QChar>

StackStack* StackStackCollectionModel::newStackStack(Interpreter::InstructionPointer* ip)
{
	emit beginInsertColumns( // TODO
	StackStack* stackStack = new StackStack(ip);
	m_stackStacks.append(stackStack);
	return stackStack;
}

StackStack* StackStackCollectionModel::deepCopy(StackStack* original)
{
	StackStack* stackStack = new StackStack(original->ip());
	
	for(StackStack::const_iterator it = original->constBegin(); it != original->constEnd(); ++it)
	{
		QStack<int>* t = new QStack<int>();

		foreach(int x, **it)
		{
			t->push(x);
		}

		stackStack->push(t);
	}
	
	m_stackStacks.append(stackStack);
	return stackStack;
}

void StackStackCollectionModel::removeStackStack(StackStack* stackStack)
{
	m_stackStacks.removeAll(stackStack);
}

QModelIndex StackStackCollectionModel::index(int row, int column, const QModelIndex& parent) const
{
	if (!parent.isValid())
	{
		// We're being asked to create an index for a StackStack
		return createIndex(row, column, m_stackStacks[row]);
	}
	
	StackStackCollectionItem* parentItem = (StackStackCollectionItem*) parent.internalPointer();
	return createIndex(row, column, parentItem->childInternalModelPointer(row));
}

QModelIndex StackStackCollectionModel::parent(const QModelIndex& index) const
{
	StackStackCollectionItem* item = (StackStackCollectionItem*) index.internalPointer();
	
	switch (item->type())
	{
	case StackStackCollectionItem::Type_DataItem:
	{
		Stack* stack = (Stack*) item->parent();
		StackStack* stackStack = (StackStack*) stack->parent();
		return createIndex(stackStack->indexOf(stack), 1, stack);
	}
	case StackStackCollectionItem::Type_Stack:
	{
		StackStack* stackStack = (StackStack*) item->parent();
		return createIndex(m_stackStacks.indexOf(stackStack), 1, stackStack);
	}
	case StackStackCollectionItem::Type_StackStack:
	default:
		return QModelIndex();
	}
}

int StackStackCollectionModel::rowCount(const QModelIndex& parent) const
{
	if (!parent.isValid())
		return m_stackStacks.count();
	
	StackStackCollectionItem* parentItem = (StackStackCollectionItem*) parent.internalPointer();
	return parentItem->count();
}

int StackStackCollectionModel::columnCount(const QModelIndex& parent) const
{
	return 1;
}

QVariant StackStackCollectionModel::data(const QModelIndex& index, int role) const
{
	if (role == Qt::DisplayRole)
	{
		StackStackCollectionItem* item = (StackStackCollectionItem*) index.internalPointer();
		
		return item->data(index.row());
	}
	return QVariant();
}

