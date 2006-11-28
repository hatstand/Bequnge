#include "stackstack.h"

#include <QChar>

StackStack::StackStack(QObject* parent)
	:QAbstractItemModel(parent)
{

}

Qt::ItemFlags StackStack::flags(const QModelIndex& index) const
{
	Q_UNUSED(index);
	return (Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
}

QModelIndex StackStack::index(int row, int column, const QModelIndex& parent) const
{
	if(!parent.isValid())
		return createIndex(row, column, at(row));
	else
		return QModelIndex();
}

QModelIndex StackStack::parent(const QModelIndex& index) const
{
	Q_UNUSED(index);
	return QModelIndex();
}

int StackStack::rowCount(const QModelIndex& parent) const
{
	if(!parent.isValid())
		return count();
	else
	{
		QStack<int>* stack = static_cast<QStack<int>*>(parent.internalPointer());
		return stack->count();
	}
}

int StackStack::columnCount(const QModelIndex &parent) const
{
	if(!parent.isValid())
		return 1;
	else
		return 2;
}

QVariant StackStack::data(const QModelIndex& index, int role) const
{
	if(!index.isValid())
		return QVariant();
	
	if(role != Qt::DisplayRole)
		return QVariant();

	QStack<int>* stack = static_cast<QStack<int>*>(index.internalPointer());
	
	switch(index.row())
	{
		case 0:
			return stack->at(index.column());
		case 1:
			return QChar(stack->at(index.column()));
		default:
			return QVariant();
	}
}

QVariant StackStack::headerData(int section, Qt::Orientation orientation, int role) const
{
	Q_UNUSED(section);
	Q_UNUSED(orientation);
	Q_UNUSED(role);
	return QVariant();
}

