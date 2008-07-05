#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "fungespace.h"

#include <QIODevice>
#include <QStack>
#include <QColor>

class StackStack;
class Stack;
class StackStackCollectionModel;

class Interpreter: public QObject
{
	Q_OBJECT

#ifdef TESTINTERPRETER_H
	friend class TestInterpreter;
#endif

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

	class InstructionPointer
	{
	public:
		InstructionPointer(StackStackCollectionModel* model, Coord position, Coord direction, Coord storageOffset, int id);
		InstructionPointer(StackStackCollectionModel* model, const InstructionPointer& ip, int id);
		~InstructionPointer();
		
		Stack* stack();

		Coord m_pos;
		Coord m_direction;
		Coord m_storageOffset;
		StackStack* m_stackStack;
		bool m_usingSecondStack;
		
		QColor m_color;
		int m_particleGroup;

		bool m_stringMode;
		bool m_commentMode;
		WaitingForInput m_waitingForInput;
		bool m_jumpedSpace;

		int m_id;
	
	private:
		void init();
	};


	Interpreter(StackStackCollectionModel* stackModel, FungeSpace* space, QObject* parent = 0);
	virtual ~Interpreter();

	void run();
	Status step();
	QList<Interpreter::Status> stepAll();
	
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
	StackStackCollectionModel* m_stackModel;
	QList<InstructionPointer*> m_ips;
	InstructionPointer* m_ip;

	QIODevice* m_input;
	QString m_version;

	FungeSpace* m_space;
	
	Status compute(QChar);
	void move(bool skipSpaces = true);
	void jumpSpaces();
	Coord getNextInstruction();


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
	void comment();
	void character();
	void storeCharacter();
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
	void getSysInfo();

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

	void loadSemantics();

	void panic(QString message = NULL);

	int m_ipid;
};

#endif

