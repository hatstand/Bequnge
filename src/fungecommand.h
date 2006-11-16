#ifndef FUNGECOMMAND_H
#define FUNGECOMMAND_H

#include "fungespace.h"

#include <QUndoCommand>

typedef QHash<Coord, QPair<QChar, QChar> > ChangeList;

class FungeCommand: public QUndoCommand
{
public:
	FungeCommand(FungeSpace* space, Coord p, QChar newChar, QUndoCommand* parent = 0);
	FungeCommand(FungeSpace* space, ChangeList changes, QUndoCommand* parent = 0);

	virtual void redo();
	virtual void undo();
private:
	FungeSpace* m_space;
	Coord m_p;
	QChar m_oldChar;
	QChar m_newChar;

	ChangeList m_changes;

	enum Type
	{
		Single,
		Multi
	};

	Type m_type;
};

#endif
