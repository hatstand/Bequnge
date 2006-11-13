#include "fungeconsole.h"

FungeConsole::FungeConsole(QWidget* parent)
	: QTextEdit(parent),
	  m_lastAnchor(0),
	  m_lastPosition(0)
{
	m_cursor = new QTextCursor(document());
}


FungeConsole::~FungeConsole()
{
}


void FungeConsole::append(QString text)
{
}


