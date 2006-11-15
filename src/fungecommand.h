#ifndef FUNGECOMMAND_H
#define FUNGECOMMAND_H

#include "fungespace.h"

#include <QUndoCommand>

class FungeCommand: public QUndoCommand
{
public:
	FungeCommand(FungeSpace* space, Coord p, QChar newChar, QUndoCommand* parent = 0);

	virtual void redo();
	virtual void undo();
private:
	FungeSpace* m_space;
	Coord m_p;
	QChar m_oldChar;
	QChar m_newChar;
};

#endif
