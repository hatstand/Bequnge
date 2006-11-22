#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "fungespace.h"
#include "stackstack.h"

#include <QIODevice>
#include <QStack>
#include <QColor>

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

	struct InstructionPointer
	{
		InstructionPointer(Coord, Coord, Coord);
		InstructionPointer(const InstructionPointer&);
		~InstructionPointer();

		Coord m_pos;
		Coord m_direction;
		Coord m_storageOffset;
		//QStack<QStack<int>* > m_stackStack;
		StackStack m_stackStack;
		QStack<int>* m_stack;
		
		QColor m_color;

		bool m_stringMode;
		WaitingForInput m_waitingForInput;
	};


	Interpreter(FungeSpace* space, QObject* parent = 0);
	virtual ~Interpreter();

	void run();
	Status step();
	Status stepAll();
	
	void pushItem(int c);
	int popItem();
	
	InstructionPointer* ip() { return m_ip; }
	
	WaitingForInput waitingFor() { return m_ip->m_waitingForInput; }
	
public slots:
	void provideInput(QChar c);
	void provideInput(int i);

signals:
	void output(QChar);
	void output(QString);
	void input(Interpreter::WaitingForInput type);
	void ipCreated(int index, Interpreter::InstructionPointer* ip);
	void ipChanged(Interpreter::InstructionPointer* ip);
	void ipDestroyed(Interpreter::InstructionPointer* ip);
	
	void stackPushed(int value);
	void stackPopped();

private:
	QList<InstructionPointer*> m_ips;
	InstructionPointer* m_ip;

	QIODevice* m_input;
	QString m_version;

	FungeSpace* m_space;
	bool m_jumpedSpace;
	
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
	void upDimension();
	void downDimension();
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

	void split();
	bool end();

	void pushNumber(QChar n);
	void pushVector(Coord c);
	Coord popVector();

	void panic(QString message = NULL);
};

#endif

