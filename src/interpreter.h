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
		Invalid,
		SuspendForInput
	};
	
	enum WaitingForInput
	{
		NotWaiting,
		WaitingForChar,
		WaitingForInteger
	};


	Interpreter(FungeSpace* space, QObject* parent = 0);
	virtual ~Interpreter();

	void run();
	Status step();
	
	void pushItem(int c);
	int popItem();
	
	Coord pcPosition(int pc) { return m_pos; }
	Coord pcDirection(int pc) { return m_direction; }
	
	WaitingForInput waitingFor() { return m_waitingForInput; }
	
public slots:
	void provideInput(QChar c);
	void provideInput(int i);

signals:
	void output(QChar);
	void output(QString);
	void input(Interpreter::WaitingForInput type);
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
	Coord m_storageOffset;

	bool m_stringMode;
	bool m_jumpedSpace;
	
	WaitingForInput m_waitingForInput;

	Status compute(QChar);
	void move();
	void jumpSpaces();


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
	void horizIf();
	void compare();
	void printChar();
	void printDec();
	Status inputDec();
	Status inputChar();
	void trampoline();
	void jump();
	void iterate();

	void beginBlock();
	void endBlock();
	void stackUnderStack();

	void getFunge();
	void putFunge();

	void pushNumber(QChar n);
	void pushVector(Coord c);
	Coord popVector();

	void panic(QString message = NULL);
};

#endif

