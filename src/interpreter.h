#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "array.h"

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
	QStack<QChar> m_stack;
	Array* m_counter;
	int m_direction;
	QString m_version;
	uint m_dimensions;
	Array zeroth;

	void parseHeader();
	void readInAll();
	void compute(Array* ptr);

	const QChar getChar(Array*);


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

