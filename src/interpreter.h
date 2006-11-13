#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "fungespace.h"

#include <QIODevice>
#include <QStack>

class Interpreter: public QObject
{
	Q_OBJECT

public:
	enum Status
	{
		Success,
		End,
		Invalid
	};


	Interpreter(FungeSpace* space, QObject* parent = 0);
	virtual ~Interpreter();

	void run();
	Status step();
	
	void provideInput(QChar);
	
	void pushItem(int c);
	int popItem();
	
	Coord pcPosition(int pc) { return m_pos; }
	Coord pcDirection(int pc) { return m_direction; }

signals:
	void output(QChar);
	void input();
	void pcChanged(Coord position, Coord direction);
	
	void stackPushed(int value);
	void stackPopped();

private:
	QIODevice* m_input;
	QStack<int>* m_stack;
	QStack<QStack<int>* > m_stackStack;
	QString m_version;

	FungeSpace* m_space;
	Coord m_pos;
	Coord m_direction;

	bool m_stringMode;
	bool m_jumpedSpace;

	QChar outputChar;

	bool compute(QChar);
	void move();
	void jumpSpaces();
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
	void absolute();
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

