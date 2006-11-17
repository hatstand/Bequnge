#include "fungecommand.h"

FungeCommand::FungeCommand(FungeSpace* space, Coord p, QChar newChar, QUndoCommand* parent)
	:QUndoCommand(parent), m_space(space), m_p(p), m_newChar(newChar), m_type(Single)
{
}

FungeCommand::FungeCommand(FungeSpace* space, ChangeList changes, QUndoCommand* parent)
	:QUndoCommand(parent), m_space(space), m_changes(changes), m_type(Multi)
{
}

void FungeCommand::redo()
{
	switch(m_type)
	{
		case Single:
		{
			m_space->setChar(m_p, m_newChar);
			break;
		}
		case Multi:
		{
			for(ChangeList::const_iterator it = m_changes.constBegin(); it != m_changes.constEnd(); ++it)
			{
				m_space->setChar(it.key(), it.value().second);
			}
			break;
		}
		default:
		{
			qDebug() << "Probably unset type at" << __FILE__ << __LINE__ << __func__;
			return;
		}
	}
}

void FungeCommand::undo()
{
	switch(m_type)
	{
		case Single:
		{
			m_space->setChar(m_p, m_oldChar);
			break;
		}
		case Multi:
		{
			for(ChangeList::const_iterator it = m_changes.constBegin(); it != m_changes.constEnd(); ++it)
			{
				m_space->setChar(it.key(), it.value().first);
			}
			break;
		}
		default:
		{
			qDebug() << "Probably unset type at" << __FILE__ << __LINE__ << __func__;
		}
	}
}
