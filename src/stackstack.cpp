#include "stackstack.h"

#include <QChar>

QString StackStack::text(int column)
{
	return "Instruction pointer " + QString::number(m_ip->m_id);
};