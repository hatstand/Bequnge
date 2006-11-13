#ifndef FUNGECONSOLE_H
#define FUNGECONSOLE_H

#include <QTextEdit>

class FungeConsole : public QTextEdit
{
public:
	FungeConsole(QWidget* parent = 0);
	~FungeConsole();
	
	void append(QString text);
	
private:
	int m_lastAnchor;
	int m_lastPosition;

	QTextCursor* m_cursor;
};

#endif
