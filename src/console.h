#include "interpreter.h"

#include <QObject>
#include <QTextStream>

class Console : public QObject
{
	Q_OBJECT
public:
	Console(Interpreter* i);
	virtual ~Console();

public slots:
	void writeOutput(QChar c);
	void writeOutput(QString s);
	void readInput(Interpreter::WaitingForInput type);

private:
	Interpreter* m_interpreter;
	QTextStream m_in;
	QTextStream m_out;

};
