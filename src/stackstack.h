#ifndef STACKSTACK_H
#define STACKSTACK_H

#include <QStack>
#include <QAbstractItemModel>

class StackStack: public QStack<QStack<int>*>, public QAbstractItemModel
{
public:
	StackStack(QObject* parent = 0);

	Qt::ItemFlags flags(const QModelIndex& index) const;
	QVariant data(const QModelIndex& index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	int rowCount(const QModelIndex& parent = QModelIndex()) const;
	int columnCount(const QModelIndex& parent = QModelIndex()) const;

	QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex& index) const;

private:

};

#endif
