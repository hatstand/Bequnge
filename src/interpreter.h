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

	void provideInput(QChar);

signals:
	void output(QChar);
	void input();

private:
	QIODevice* m_input;
	QStack<int>* m_stack;
	QStack<QStack<int>* > m_stackStack;
	int m_direction;
	QString m_version;
	uint m_dimensions;

	FungeSpace* m_space;
	Coord m_pos;

	bool m_stringMode;
	bool m_jumpedSpace;

	QChar outputChar;

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
	void character();
	void duplicate();
	void pop();
	void swap();
	void clear();
	void vertIf();
	void compare();
	void printChar();
	void printDec();
	void inputDec();
	void inputChar();
	void trampoline();
	void jump();
	void iterate();

	void beginBlock();
	void endBlock();


	void pushNumber(QChar n);

	void panic(QString message = NULL);
};

#endif

