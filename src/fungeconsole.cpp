#include "fungeconsole.h"

#include <QKeyEvent>
#include <QScrollBar>

FungeConsole::FungeConsole(QWidget* parent)
	: QTextEdit(parent),
	  m_lastAnchor(0),
	  m_lastLength(0),
	  m_numberParser("^[^\\d]*([\\d]+)[^\\d]")
{
	m_cursor = new QTextCursor(document());
	
	m_newFormat.setFontWeight(QFont::Bold);
	m_newFormat.setForeground(QBrush(Qt::red));
	m_inputFormat.setForeground(QBrush(Qt::darkGreen));
}


FungeConsole::~FungeConsole()
{
}


void FungeConsole::append(QString text)
{
	// Un-bold the last insertion
	m_cursor->setPosition(m_lastAnchor);
	m_cursor->movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, m_lastLength);
	m_cursor->setCharFormat(m_normalFormat);
	
	// Insert the text and make it bold
	m_cursor->movePosition(QTextCursor::End);
	m_lastAnchor = m_cursor->position();
	m_cursor->insertText(text, m_newFormat);
	m_lastLength = m_cursor->position() - m_lastAnchor;
	
	scrollToBottom();
}

void FungeConsole::getInput(Interpreter::WaitingForInput type)
{
	m_waitingForInput = type;
	send();
}

void FungeConsole::keyPressEvent(QKeyEvent* event)
{
	QString text = event->text();
	
	if (text.isEmpty())
	{
		QTextEdit::keyPressEvent(event);
		return;
	}
	
	m_cursor->movePosition(QTextCursor::End);
	m_lastAnchor = m_cursor->position();
	m_cursor->insertText(text, m_inputFormat);
	m_lastLength = m_cursor->position() - m_lastAnchor;
	
	scrollToBottom();
	
	m_buffer.append(text[0]);
	send();
}

void FungeConsole::send()
{
	switch (m_waitingForInput)
	{
	case Interpreter::NotWaiting:
		return;
	case Interpreter::WaitingForChar:
		if (!m_buffer.isEmpty())
		{
			emit charEntered(m_buffer[0]);
			m_buffer.remove(0, 1);
			m_waitingForInput = Interpreter::NotWaiting;
		}
		break;
	case Interpreter::WaitingForInteger:
		if (m_numberParser.indexIn(m_buffer) != -1)
		{
			emit intEntered(m_numberParser.cap(1).toInt());
			m_buffer.remove(0, m_numberParser.matchedLength());
			m_waitingForInput = Interpreter::NotWaiting;
		}
		break;
	}
}

void FungeConsole::scrollToBottom()
{
	QTextCursor cursor = textCursor();
	cursor.movePosition(QTextCursor::End);
	setTextCursor(cursor);
}


