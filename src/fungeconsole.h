#ifndef FUNGECONSOLE_H
#define FUNGECONSOLE_H

#include <QTextEdit>

#include "interpreter.h"

class QKeyEvent;

class FungeConsole : public QTextEdit
{
	Q_OBJECT
	
public:
	FungeConsole(QWidget* parent = 0);
	~FungeConsole();
	
public slots:
	void append(QString text);
	void getInput(Interpreter::WaitingForInput type);
	
signals:
	void charEntered(QChar c);
	void intEntered(int i);
	
private:
	void keyPressEvent(QKeyEvent* event);
	void send();
	void scrollToBottom();
	
private:
	int m_lastAnchor;
	int m_lastLength;
	Interpreter::WaitingForInput m_waitingForInput;
	
	QTextCursor* m_cursor;
	QTextCharFormat m_normalFormat;
	QTextCharFormat m_newFormat;
	QTextCharFormat m_inputFormat;
	
	QString m_buffer;
	
	QRegExp m_numberParser;
};

#endif
