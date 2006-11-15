#include "fungecommand.h"

FungeCommand::FungeCommand(FungeSpace* space, Coord p, QChar newChar, QUndoCommand* parent)
	:m_space(space), m_p(p), m_newChar(newChar), QUndoCommand(parent)
{
	m_oldChar = m_space->getChar(p);
}

void FungeCommand::redo()
{
	m_space->setChar(m_p, m_newChar);
}

void FungeCommand::undo()
{
	m_space->setChar(m_p, m_oldChar);
}
