#include "interpreter.h"

#include <QDebug>
#include <QStringList>

#include <stdlib.h>
#include <time.h>
#include <unistd.h>

Interpreter::InstructionPointer::InstructionPointer(Coord pos, Coord dir, Coord store)
	:m_pos(pos), m_direction(dir), m_storageOffset(store), 
	m_stringMode(false), m_waitingForInput(NotWaiting) 
{
	QStack<int>* t = new QStack<int>();
	m_stack = t;
	m_stackStack.push(t);
}

Interpreter::InstructionPointer::InstructionPointer(const Interpreter::InstructionPointer& ip)
	:m_pos(ip.m_pos), m_direction(ip.m_direction), m_storageOffset(ip.m_storageOffset),
	m_stringMode(ip.m_stringMode), m_commentMode(ip.m_commentMode), m_waitingForInput(ip.m_waitingForInput) 
{
	// Deep copy the stack stack
	/*foreach(QStack<int>* i, ip.m_stackStack)
	{
		QStack<int>* t = new QStack<int>();
		qCopy(i->begin(), i->end(), t->begin());
		m_stackStack.push(t);
	}*/

	for(StackStack::const_iterator it = ip.m_stackStack.constBegin(); it != ip.m_stackStack.constEnd(); ++it)
	{
		QStack<int>* t = new QStack<int>();

		foreach(int x, **it)
		{
			t->push(x);
		}

		m_stackStack.push(t);
	}

	m_stack = m_stackStack.top();
}

Interpreter::InstructionPointer::~InstructionPointer()
{
	for(StackStack::iterator it = m_stackStack.begin(); it != m_stackStack.end(); ++it)
		delete(*it);
}

Interpreter::Interpreter(FungeSpace* space, QObject* parent)
	: QObject(parent),m_space(space)
{
	m_version = "1";

	Coord direction;
	direction[0] = 1;
	m_ip = new InstructionPointer(Coord(), direction, Coord());

	m_ip->m_stringMode = false;
	m_ip->m_commentMode = false;

	m_ips << m_ip;

	srand((int)time(NULL) / (int)getpid());
}

Interpreter::~Interpreter()
{
	foreach(InstructionPointer* ip, m_ips)
	{
		delete ip;
	}
}

void Interpreter::jumpSpaces()
{
	QChar t = m_space->getChar(m_ip->m_pos);

	if(t.category() == QChar::Separator_Space || (m_ip->m_commentMode && t != ';'))
	{
		m_jumpedSpace = true;	
		move();
	}
}

void Interpreter::move()
{
	if (m_ip == NULL)
		return;
	
	for(uint i = 0; i < m_space->dimensions(); ++i)
	{
		m_ip->m_pos[i] += m_ip->m_direction[i];
		if(m_ip->m_pos[i] > m_space->getPositiveEdge(i))
			m_ip->m_pos[i] = m_space->getNegativeEdge(i);
		else if(m_ip->m_pos[i] < m_space->getNegativeEdge(i))
			m_ip->m_pos[i] = m_space->getPositiveEdge(i);
	}

	jumpSpaces();

	//qDebug() << "Moved to:" << m_pos;
	emit ipChanged(m_ip);
}

Interpreter::Status Interpreter::step()
{
	if(m_jumpedSpace)
	{
		m_jumpedSpace = false;
		if(m_ip->m_stringMode)
			pushItem(QChar(' ').unicode());
	}
	QChar c = m_space->getChar(m_ip->m_pos);
	Interpreter::Status ret = compute(c);

	//qDebug() << "Direction: " << m_direction;
	if(ret == Success)
	{
		move();
		return Success;
	}

	return ret;
}

Interpreter::Status Interpreter::stepAll()
{
	foreach(InstructionPointer* ip, m_ips)
	{
		m_ip = ip;
		Interpreter::Status ret = step();
		if (ret != Success)
			return ret;
	}

	return Success;
}

void Interpreter::run()
{
	while(step());
}

// Call this with a QChar Array
Interpreter::Status Interpreter::compute(QChar command)
{
	//qDebug() << "Instruction:" << command;
	if(m_ip->m_stringMode && command != '"')
	{
		pushItem(command.unicode());
		return Success;
	}
	
	if (m_ip->m_waitingForInput != NotWaiting)
		return SuspendForInput;

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
	else if(command == 'z')
		upDimension();
	else if(command == 'y')
		downDimension();
	else if(command == 'r')
		reverse();
	else if(command == 'x')
		absolute();
	else if(command == '"')
		string();
	else if(command == ';')
		comment();
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
	else if(command == '_')
		horizIf();
	else if(command == 'w')
		compare();
	else if(command == ',')
		printChar();
	else if(command == '.')
		printDec();
	else if(command == '&')
		return inputDec();
	else if(command == '~')
		return inputChar();
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
	else if(command == 't')
		split();
	else if(command.isNumber())
		pushNumber(command);
	else if(command == 'a')
		pushNumber(10);
	else if(command == 'b')
		pushNumber(11);
	else if(command == 'c')
		pushNumber(12);
	else if(command == 'd')
		pushNumber(13);
	else if(command == 'e')
		pushNumber(14);
	else if(command == 'f')
		pushNumber(15);
	else if(command == '@')
	{
		if (end())
			return End;
	}
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
		pushItem(0);
	else
		pushItem(1);
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
	m_ip->m_direction[0] = 0;
	m_ip->m_direction[1] = -1;

	for(uint i = 2; i < m_space->dimensions(); ++i)
		m_ip->m_direction[i] = 0;
}

void Interpreter::right()
{
	m_ip->m_direction[0] = 1;

	for(uint i = 1; i < m_space->dimensions(); ++i)
		m_ip->m_direction[i] = 0;
}

void Interpreter::left()
{
	m_ip->m_direction[0] = -1;

	for(uint i = 1; i < m_space->dimensions(); ++i)
		m_ip->m_direction[i] = 0;
}

void Interpreter::down()
{
	m_ip->m_direction[0] = 0;
	m_ip->m_direction[1] = 1;

	for(uint i = 2; i < m_space->dimensions(); ++i)
		m_ip->m_direction[i] = 0;
}

void Interpreter::higher()
{
	m_ip->m_direction[0] = 0;
	m_ip->m_direction[1] = 0;
	m_ip->m_direction[2] = 1;

	for(uint i = 3; i < m_space->dimensions(); ++i)
		m_ip->m_direction[i] = 0;
}

void Interpreter::lower()
{
	m_ip->m_direction[0] = 0;
	m_ip->m_direction[1] = 0;
	m_ip->m_direction[2] = -1;

	for(uint i = 3; i < m_space->dimensions(); ++i)
		m_ip->m_direction[i] = 0;
}

void Interpreter::random()
{
	int dim = rand() % m_space->dimensions();
	int dir = rand() % 1;

	m_ip->m_direction[dim] = (dir == 1) ? dir : -1;
}

void Interpreter::turnLeft()
{
	Q_ASSERT(m_space->dimensions() >= 2);

	// TODO
	m_ip->m_direction[0] = -m_ip->m_direction[1];
	m_ip->m_direction[1] = m_ip->m_direction[0];
}

void Interpreter::turnRight()
{
	Q_ASSERT(m_space->dimensions() >= 2);

	// TODO
	m_ip->m_direction[0] = m_ip->m_direction[1];
	m_ip->m_direction[1] = -m_ip->m_direction[0];
}

void Interpreter::upDimension()
{
	int dim = popItem();
	Q_ASSERT((uint)dim < m_space->dimensions());

	for(uint i = 0; i < m_space->dimensions(); ++i)
		m_ip->m_direction[i] = 0;
	
	m_ip->m_direction[dim] = 1;
}

void Interpreter::downDimension()
{
	int dim = popItem();
	Q_ASSERT((uint)dim < m_space->dimensions());

	for(uint i = 0; i < m_space->dimensions(); ++i)
		m_ip->m_direction[i] = 0;

	m_ip->m_direction[dim] = -1;
}

void Interpreter::reverse()
{
	for(uint i = 0; i < m_space->dimensions(); ++i)
		m_ip->m_direction[i] *= -1;
}

void Interpreter::absolute()
{
	for(int i = m_space->dimensions() - 1; i > 0; --i)
	{
		m_ip->m_direction[i] = popItem();
	}
}

void Interpreter::string()
{
	// For null-terminated strings?
	//if(!m_ip->m_stringMode)
	//	pushItem('\0');

	m_ip->m_stringMode = !m_ip->m_stringMode;
}

void Interpreter::comment()
{
	m_ip->m_commentMode = !m_ip->m_commentMode;
}

void Interpreter::character()
{
	move();
	pushItem(QChar('\0').unicode());
	pushItem(m_space->getChar(m_ip->m_pos).unicode());
}

void Interpreter::duplicate()
{
	int x = popItem();
	pushItem(x);
	pushItem(x);
}

void Interpreter::pop()
{
	m_ip->m_stack->pop();
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
	m_ip->m_stack->clear();
}

void Interpreter::vertIf()
{
	int x = popItem();
	if(x)
		up();
	else
		down();
}

void Interpreter::horizIf()
{
	int x = popItem();
	if(x)
		left();
	else
		right();
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
	QString outputStr = QString::number(popItem());
	qDebug() << outputStr;
	emit(output(outputStr));
}

Interpreter::Status Interpreter::inputChar()
{
	m_ip->m_waitingForInput = WaitingForChar;
	emit input(m_ip->m_waitingForInput);
	
	return SuspendForInput;
}

Interpreter::Status Interpreter::inputDec()
{
	m_ip->m_waitingForInput = WaitingForInteger;
	emit input(m_ip->m_waitingForInput);
	
	return SuspendForInput;
}

void Interpreter::provideInput(QChar c)
{
	provideInput(c.unicode());
}

void Interpreter::provideInput(int i)
{
	if (m_ip->m_waitingForInput == NotWaiting)
		return;
	pushItem(i);
	
	m_ip->m_waitingForInput = NotWaiting;
	
	move();
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
		compute(m_space->getChar(m_ip->m_pos));
}

void Interpreter::beginBlock()
{
	int x = popItem();
	QStack<int>* newStack;

	int s = m_ip->m_stack->size();
	newStack = new QStack<int>();

	if(s >= x)
	{
		foreach(int o, m_ip->m_stack->mid(s-x))
		{
			newStack->push(o);
		}
	}
	else if(x < 0)
	{
		for(int i = 0; i < qAbs(x); ++i)
			m_ip->m_stack->push(QChar(' ').unicode());
	}
	else
	{
		foreach(int o, m_ip->m_stack->mid(0))
			newStack->push(o);

		for(int i = 0; i < x - s; ++i)
			newStack->push(QChar(' ').unicode());
	}

	pushVector(m_ip->m_storageOffset);

	for(uint i = 0; i < m_space->dimensions(); ++i)
		m_ip->m_storageOffset[i] = m_ip->m_pos[i] + m_ip->m_direction[i];

	m_ip->m_stack = newStack;
	m_ip->m_stackStack.push(m_ip->m_stack);
}

void Interpreter::endBlock()
{
	int x = qAbs(popItem());
	
	QStack<int>* oldStack = m_ip->m_stackStack.pop();
	m_ip->m_stack = m_ip->m_stackStack.top();
	m_ip->m_storageOffset = popVector();
	int s = oldStack->size();

	if(s >= x)
	{
		foreach(int i, oldStack->mid(s-x))
		{
			m_ip->m_stack->push(i);
		}
	}
	else
	{
		for(int i = 0; i < x-s; ++i)
			m_ip->m_stack->push(QChar(' ').unicode());

		foreach(int i, *oldStack)
		{
			m_ip->m_stack->push(i);
		}
	}


	delete(oldStack);
}

void Interpreter::stackUnderStack()
{
	if(m_ip->m_stackStack.size() == 1)
		reverse();

	int n = qAbs(popItem());
	if(n == 0)
		return;

	QStack<int>* over = m_ip->m_stackStack.pop();
	QStack<int>* under = m_ip->m_stackStack.top();

	m_ip->m_stackStack.push(over);

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
		c[i] += m_ip->m_storageOffset[i];

	pushItem(m_space->getChar(c).unicode());
}

void Interpreter::putFunge()
{
	Coord c = popVector();
	for(uint i = 0; i < m_space->dimensions(); ++i)
		c[i] += m_ip->m_storageOffset[i];

	m_space->setChar(c, popItem());
}

void Interpreter::pushNumber(QChar n)
{
	pushItem(QString(n).toInt());
}

void Interpreter::pushItem(int c)
{
	m_ip->m_stack->push(c);
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
	if(m_ip->m_stack->isEmpty())
		return 0;

	int n = m_ip->m_stack->pop();
	
	emit stackPopped();
	
	return n;
}

void Interpreter::split()
{
	InstructionPointer* t = new InstructionPointer(*m_ip);
	m_ips.prepend(t);

	qSwap(m_ip, t);
	reverse();
	move();
	qSwap(m_ip, t);
	
	emit ipCreated(m_ips.indexOf(t), t);
}

bool Interpreter::end()
{
	int index = m_ips.indexOf(m_ip);
	m_ips.removeAt(index);
	emit ipDestroyed(m_ip);
	delete m_ip;
	m_ip = NULL;
	if (m_ips.count() == 0)
		return true;
	
	/*if (index >= m_ips.count())
		m_ip = m_ips.first();
	else
		m_ip = m_ips[index];*/
	
	return false;
}

void Interpreter::panic(QString message)
{
	message = "PANIC!: " + message;
	qDebug() << m_ip->m_pos[0] << m_ip->m_pos[1];
	qFatal(message.toAscii());
}
