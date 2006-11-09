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
	FungeSpace<2>::Coord m_pos;

	int m_edgeLeft;
	int m_edgeRight;
	int m_edgeTop;
	int m_edgeBottom;

	bool m_stringMode;
	bool m_jumpedSpace;


	void parseHeader();
	void readInAll();
	bool compute(QChar);
	void move();
	void jumpSpaces();
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
	void up();
	void right();
	void left();
	void down();
	void higher();
	void lower();
	void random();
	void turnLeft();
	void turnRight();
	void reverse();
	void string();
	void duplicate();
	void vertIf();
	void printChar();


	void pushNumber(QChar n);

	void panic(QString message = NULL);
};

#endif

