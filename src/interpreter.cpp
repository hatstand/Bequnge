#include "interpreter.h"

#include <QDebug>
#include <QStringList>

#include <stdlib.h>
#include <time.h>
#include <unistd.h>

Interpreter::Interpreter(FungeSpace* space, QObject* parent)
	: QObject(parent),m_space(space),
	m_pos(m_space->dimensions(), 0),
	m_direction(m_space->dimensions(), 0),
	m_storageOffset(m_space->dimensions(), 0)
{
	m_version = "1";

	m_stringMode = false;

	m_stack = new QStack<int>();

	m_stackStack.push(m_stack);

	m_direction[0] = 1;

	srand((int)time(NULL) / (int)getpid());
}

Interpreter::~Interpreter()
{
	foreach(QStack<int>* i, m_stackStack)
	{
		delete(i);
	}
}

void Interpreter::jumpSpaces()
{
	if(m_space->getChar(m_pos).category() == QChar::Separator_Space)
	{
		m_jumpedSpace = true;	
		move();
	}
}

void Interpreter::move()
{
	for(uint i = 0; i < m_space->dimensions(); ++i)
	{
		m_pos[i] += m_direction[i];
		if(m_pos[i] > m_space->getPositiveEdge(i))
			m_pos[i] = m_space->getNegativeEdge(i);
		else if(m_pos[i] < m_space->getNegativeEdge(i))
			m_pos[i] = m_space->getPositiveEdge(i);
	}

	jumpSpaces();

	//qDebug() << "Moved to:" << m_pos;
	emit pcChanged(m_pos, m_direction);
}

Interpreter::Status Interpreter::step()
{
	if(m_jumpedSpace)
	{
		m_jumpedSpace = false;
		if(m_stringMode)
			pushItem(QChar(' ').unicode());
	}
	QChar c = m_space->getChar(m_pos);
	bool ret = compute(c);

	//qDebug() << "Direction: " << m_direction;
	if(ret == Success)
	{
		move();
		return Success;
	}

	return Invalid;
}

void Interpreter::run()
{
	while(step());
}

// Call this with a QChar Array
Interpreter::Status Interpreter::compute(QChar command)
{
	//qDebug() << "Instruction:" << command;
	if(m_stringMode && command != '"')
	{
		pushItem(command.unicode());
		return Success;
	}

	if(command == '+')
		add();
	else if(command == '-')
		subtract();
	else if(command == '*')
		multiply();
	else if(command == '/')
		divide();
	else if(command == '%')
		modulo();
	else if(command == '!')
		notf();
	else if(command == '`')
		greaterThan();
	else if(command == '^')
		up();
	else if(command == '>')
		right();
	else if(command == '<')
		left();
	else if(command == 'v')
		down();
	else if(command == 'h')
		higher();
	else if(command == 'l')
		lower();
	else if(command == '?')
		random();
	else if(command == '[')
		turnLeft();
	else if(command == ']')
		turnRight();
	else if(command == 'r')
		reverse();
	else if(command == 'x')
		absolute();
	else if(command == '"')
		string();
	else if(command == '\'')
		character();
	else if(command == ':')
		duplicate();
	else if(command == '$')
		pop();
	else if(command == '\\')
		swap();
	else if(command == 'n')
		clear();
	else if(command == '|')
		vertIf();
	else if(command == 'w')
		compare();
	else if(command == ',')
		printChar();
	else if(command == '.')
		printDec();
	else if(command == '&')
		inputDec();
	else if(command == '~')
		inputChar();
	else if(command == '#')
		trampoline();
	else if(command == 'j')
		jump();
	else if(command == 'k')
		iterate();
	else if(command == '{')
		beginBlock();
	else if(command == '}')
		endBlock();
	else if(command == 'u')
		stackUnderStack();
	else if(command == 'p')
		putFunge();
	else if(command == 'g')
		getFunge();
	else if(command.isNumber())
		pushNumber(command);
	else if(command == '@')
		return End;
	else
		return Invalid;
		//panic("Don't understand character: " + QString(command));

	return Success;
}

//Instructions
void Interpreter::add()
{
	int x = popItem();
	int y = popItem();

	int z = y + x;

	qDebug() << x << "+" << y << "=" << z;
	pushItem(z);
}

void Interpreter::subtract()
{
	int x = popItem();
	int y = popItem();

	int z = y - x;

	qDebug() << y << "-" << x << "=" << z;
	pushItem(z);
}

void Interpreter::multiply()
{
	int x = popItem();
	int y = popItem();

	int z = y * x;

	qDebug() << y << "*" << x << "=" << z;
	pushItem(z);
}

void Interpreter::divide()
{
	int x = popItem();
	int y = popItem();

	int z = y * x;

	qDebug() << y << "/" << x << "=" << z;
	pushItem(z);
}

void Interpreter::modulo()
{
	int x = popItem();
	int y = popItem();

	int z = y % x;

	qDebug() << y << "%" << x << "=" << z;
	pushItem(z);
}

void Interpreter::notf()
{
	int x = popItem();
	if(x)
		pushItem(1);
	else
		pushItem(0);
}

void Interpreter::greaterThan()
{
	int x = popItem();
	int y = popItem();

	if(y > x)
		pushItem(1);
	else
		pushItem(0);
}

void Interpreter::up()
{
	Q_ASSERT(m_direction.count() >= 2);

	m_direction[0] = 0;
	m_direction[1] = -1;

	for(uint i = 2; i < m_space->dimensions(); ++i)
		m_direction[i] = 0;
}

void Interpreter::right()
{
	m_direction[0] = 1;

	for(uint i = 1; i < m_space->dimensions(); ++i)
		m_direction[i] = 0;
}

void Interpreter::left()
{
	m_direction[0] = -1;

	for(uint i = 1; i < m_space->dimensions(); ++i)
		m_direction[i] = 0;
}

void Interpreter::down()
{
	Q_ASSERT(m_direction.count() >= 2);

	m_direction[0] = 0;
	m_direction[1] = 1;

	for(uint i = 2; i < m_space->dimensions(); ++i)
		m_direction[i] = 0;
}

void Interpreter::higher()
{
	Q_ASSERT(m_direction.count() >= 3);

	m_direction[0] = 0;
	m_direction[1] = 0;
	m_direction[2] = 1;

	for(uint i = 3; i < m_space->dimensions(); ++i)
		m_direction[i] = 0;
}

void Interpreter::lower()
{
	Q_ASSERT(m_direction.count() >= 3);

	m_direction[0] = 0;
	m_direction[1] = 0;
	m_direction[2] = -1;

	for(uint i = 3; i < m_space->dimensions(); ++i)
		m_direction[i] = 0;
}

void Interpreter::random()
{
	int dim = rand() % m_space->dimensions();
	int dir = rand() % 1;

	m_direction[dim] = (dir == 1) ? dir : -1;
}

void Interpreter::turnLeft()
{
	Q_ASSERT(m_space->dimensions() >= 2);

	m_direction[0] = -m_direction[1];
	m_direction[1] = m_direction[0];
}

void Interpreter::turnRight()
{
	Q_ASSERT(m_space->dimensions() >= 2);

	m_direction[0] = m_direction[1];
	m_direction[1] = -m_direction[0];
}

void Interpreter::reverse()
{
	for(uint i = 0; i < m_space->dimensions(); ++i)
		m_direction[i] *= -1;
}

void Interpreter::absolute()
{
	for(int i = m_space->dimensions() - 1; i > 0; --i)
	{
		m_direction[i] = popItem();
	}
}

void Interpreter::string()
{
	// Huh?
	//if(!m_stringMode)
	//	pushItem('\0');

	m_stringMode = !m_stringMode;
}

void Interpreter::character()
{
	move();
	pushItem(QChar('\0').unicode());
	pushItem(m_space->getChar(m_pos).unicode());
}

void Interpreter::duplicate()
{
	int x = popItem();
	pushItem(x);
	pushItem(x);
}

void Interpreter::pop()
{
	m_stack->pop();
}

void Interpreter::swap()
{
	int a = popItem();
	int b = popItem();

	pushItem(a);
	pushItem(b);
}

void Interpreter::clear()
{
	m_stack->clear();
}

void Interpreter::vertIf()
{
	int x = popItem();
	if(x)
		up();
	else
		down();
}

void Interpreter::compare()
{
	int b = popItem();
	int a = popItem();

	if(a < b)
		turnLeft();
	else if(a > b)
		turnRight();
}

void Interpreter::printChar()
{
	QChar outputChar = QChar(popItem());
	qDebug() << outputChar;
	emit(output(outputChar));
}

void Interpreter::printDec()
{
	QChar outputChar = QString::number(popItem())[0];
	qDebug() << outputChar;
	emit(output(outputChar));
}

void Interpreter::inputChar()
{

}

void Interpreter::inputDec()
{

}

void Interpreter::trampoline()
{
	move();
}

void Interpreter::jump()
{
	int x = popItem();

	if(x > 0)
	{
		for(int i = 0; i < x; ++i)
			move();
	}
	else if(x < 0)
	{
		reverse();
		for(int i = x; i > 0; --i)
			move();

		reverse();
	}
}

void Interpreter::iterate()
{
	int x = popItem();
	move();

	for(int i = 0; i < x; ++i)
		compute(m_space->getChar(m_pos));
}

void Interpreter::beginBlock()
{
	int x = popItem();
	QStack<int>* newStack;

	int s = m_stack->size();
	newStack = new QStack<int>();

	if(s >= x)
	{
		foreach(int o, m_stack->mid(s-x))
		{
			newStack->push(o);
		}
	}
	else if(x < 0)
	{
		for(int i = 0; i < qAbs(x); ++i)
			m_stack->push(QChar(' ').unicode());
	}
	else
	{
		foreach(int o, m_stack->mid(0))
			newStack->push(o);

		for(int i = 0; i < x - s; ++i)
			newStack->push(QChar(' ').unicode());
	}

	pushVector(m_storageOffset);

	for(uint i = 0; i < m_space->dimensions(); ++i)
		m_storageOffset[i] = m_pos[i] + m_direction[i];

	m_stack = newStack;
	m_stackStack.push(m_stack);
}

void Interpreter::endBlock()
{
	int x = qAbs(popItem());
	
	QStack<int>* oldStack = m_stackStack.pop();
	m_stack = m_stackStack.top();
	m_storageOffset = popVector();
	int s = oldStack->size();

	if(s >= x)
	{
		foreach(int i, oldStack->mid(s-x))
		{
			m_stack->push(i);
		}
	}
	else
	{
		for(int i = 0; i < x-s; ++i)
			m_stack->push(QChar(' ').unicode());

		foreach(int i, *oldStack)
		{
			m_stack->push(i);
		}
	}


	delete(oldStack);
}

void Interpreter::stackUnderStack()
{
	if(m_stackStack.size() == 1)
		reverse();

	int n = qAbs(popItem());
	if(n == 0)
		return;

	QStack<int>* over = m_stackStack.pop();
	QStack<int>* under = m_stackStack.top();

	m_stackStack.push(over);

	while(n > 0 && !under->isEmpty())
	{
		over->push(under->pop());
		--n;
	}
}

void Interpreter::getFunge()
{
	Coord c = popVector();
	for(uint i = 0; i < m_space->dimensions(); ++i)
		c[i] += m_storageOffset[i];

	pushItem(m_space->getChar(c).unicode());
}

void Interpreter::putFunge()
{
	Coord c = popVector();
	for(uint i = 0; i < m_space->dimensions(); ++i)
		c[i] += m_storageOffset[i];

	m_space->setChar(c, popItem());
}

void Interpreter::pushNumber(QChar n)
{
	pushItem(QString(n).toInt());
}

void Interpreter::pushItem(int c)
{
	m_stack->push(c);
	emit stackPushed(c);
}

void Interpreter::pushVector(Coord c)
{
	foreach(int x, c)
	{
		pushItem(x);
	}
}

Coord Interpreter::popVector()
{
	Coord c;
	QStack<int> t;

	for(uint i = 0; i < m_space->dimensions(); ++i)
		t.push(popItem());

	while(!t.isEmpty())
	{
		c << t.pop();
	}

	return c;
}

int Interpreter::popItem()
{
	if(m_stack->isEmpty())
		return 0;

	int n = m_stack->pop();
	
	emit stackPopped();
	
	return n;
}

void Interpreter::panic(QString message)
{
	message = "PANIC!: " + message;
	qDebug() << m_pos[0] << m_pos[1];
	qFatal(message.toAscii());
}
