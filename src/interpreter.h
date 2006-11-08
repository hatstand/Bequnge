#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "fungespace.h"

#include <QIODevice>
#include <QStack>

class Interpreter: public QObject
{
	Q_OBJECT

public:
	Interpreter(QIODevice* input, QObject* parent = 0);
	virtual ~Interpreter();

	void parse();
	void run();

private:
	QIODevice* m_input;
	QStack<int> m_stack;
	int m_direction;
	QString m_version;
	uint m_dimensions;

	FungeSpace<2> m_space;
	int m_pos[2];


	void parseHeader();
	void readInAll();
	bool compute(QChar);
	bool step();
	void getNext();


	// Instructions
	void add();
	void subtract();
	void multiply();
	void divide();
	void modulo();
	void greaterThan();
	void notf();

	void pushNumber(QChar n);

	void panic();
};

#endif

