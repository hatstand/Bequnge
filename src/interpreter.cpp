#include "interpreter.h"

#include <QDebug>
#include <QStringList>

Interpreter::Interpreter(QIODevice* input, QObject* parent)
	: QObject(parent),m_input(input),m_space(NULL)
{
	m_version = "1";
	m_dimensions = 2;
	m_direction = 1;

	m_stringMode = false;

	m_stack = new QStack<int>();

	m_stackStack.push(m_stack);
}

Interpreter::~Interpreter()
{
}

void Interpreter::parseHeader()
{
	qDebug() << "Parsing header";

	QString line;
	while(((line = m_input->readLine()).trimmed().isEmpty()));

	QStringList args = line.split(",");
	foreach(QString i, args)
	{
		QStringList t = i.split(" ");
		if(t.size() != 2)
		{
			qFatal("Expected space separated key->value");
		}
		else
		{
			if(t[0].toLower() == "version")
			{
				m_version = t[1];
			}
			else if(t[0].toLower() == "dimensions")
			{
				bool ok = false;
				m_dimensions = t[1].toUInt(&ok);
				if(!ok)
				{
					qFatal("Expected unsigned integer for value of dimensions");
				}
			}
		}
	}

	m_space = new FungeSpace(m_dimensions);
	for(uint i = 0; i < m_dimensions; ++i)
		m_pos << 0;

	qDebug() << "Finished parsing header";
}

void Interpreter::readInAll()
{
	qDebug() << "Reading in code";
	QString line;


	Coord pos;
	for(uint i = 0; i < m_dimensions; ++i)
		pos << 0;

	while((line = m_input->readLine()) != NULL)
	{
		int i = 0;
		for(; i < line.length(); ++i)
		{
			if(line[i] == '\n')
				break;

			pos[0] = i;
			//qDebug() << "Putting:" << line[i] << "in:" << pos[0] << pos[1];
			m_space->setChar(pos, line[i]);
		}

		++pos[1];
	}

	qDebug() << "Finished reading code";
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
	for(uint i = 0; i < m_dimensions; ++i)
	{
		if(qAbs(m_direction)-1 == i)
		{
			if(m_direction > 0)
			{
				++m_pos[i];
				if(m_pos[i] > m_space->getPositiveEdge(i))
					m_pos[i] = m_space->getNegativeEdge(i);
			}
			else
			{
				--m_pos[i];
				if(m_pos[i] < m_space->getNegativeEdge(i))
					m_pos[i] = m_space->getPositiveEdge(i);
			}
		}
	}

	//qDebug() << "Moved to:" << m_pos[0] << m_pos[1] << m_space->getChar(m_pos);
	jumpSpaces();
}

bool Interpreter::step()
{
	if(m_jumpedSpace)
	{
		m_jumpedSpace = false;
		if(m_stringMode)
			m_stack->push(QChar(' ').unicode());
	}
	QChar c = m_space->getChar(m_pos);
	bool ret = compute(c);

	//qDebug() << "Direction: " << m_direction;
	move();

	return ret;
}

void Interpreter::run()
{
	while(step());
}

void Interpreter::getNext()
{
}

// Call this with a QChar Array
bool Interpreter::compute(QChar command)
{
	//qDebug() << "Instruction:" << command;
	if(m_stringMode && command != '"')
	{
		m_stack->push(command.unicode());
		return true;
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
	else if(command.isNumber())
		pushNumber(command);
	else if(command == '@')
		return false;
	else
		panic("Don't understand character: " + QString(command));

	return true;
}

void Interpreter::parse()
{
	m_input->open(QIODevice::ReadOnly);
	parseHeader();

	readInAll();
	m_input->close();
}


//Instructions
void Interpreter::add()
{
	int x = m_stack->pop();
	int y = m_stack->pop();

	int z = y + x;

	qDebug() << x << "+" << y << "=" << z;
	m_stack->push(z);
}

void Interpreter::subtract()
{
	int x = m_stack->pop();
	int y = m_stack->pop();

	int z = y - x;

	qDebug() << y << "-" << x << "=" << z;
	m_stack->push(z);
}

void Interpreter::multiply()
{
	int x = m_stack->pop();
	int y = m_stack->pop();

	int z = y * x;

	qDebug() << y << "*" << x << "=" << z;
	m_stack->push(z);
}

void Interpreter::divide()
{
	int x = m_stack->pop();
	int y = m_stack->pop();

	int z = y * x;

	qDebug() << y << "/" << x << "=" << z;
	m_stack->push(z);
}

void Interpreter::modulo()
{
	int x = m_stack->pop();
	int y = m_stack->pop();

	int z = y % x;

	qDebug() << y << "%" << x << "=" << z;
	m_stack->push(z);
}

void Interpreter::notf()
{
	int x = m_stack->pop();
	if(x)
		m_stack->push(1);
	else
		m_stack->push(0);
}

void Interpreter::greaterThan()
{
	int x = m_stack->pop();
	int y = m_stack->pop();

	if(y > x)
		m_stack->push(1);
	else
		m_stack->push(0);
}

void Interpreter::up()
{
	m_direction = -2;
}

void Interpreter::right()
{
	m_direction = 1;
}

void Interpreter::left()
{
	m_direction = -1;
}

void Interpreter::down()
{
	m_direction = 2;
}

void Interpreter::higher()
{
	m_direction = -3;
}

void Interpreter::lower()
{
	m_direction = 3;
}

void Interpreter::random()
{
	
}

void Interpreter::turnLeft()
{
	switch(m_direction)
	{
		case 1:
			m_direction = -2;
			break;
		case 2:
			m_direction = 1;
			break;
		case -1:
			m_direction = 2;
			break;
		case -2:
			m_direction = -1;
			break;
	}
}

void Interpreter::turnRight()
{
	switch(m_direction)
	{
		case 1:
			m_direction = 2;
			break;
		case 2:
			m_direction = -1;
			break;
		case -1:
			m_direction = -2;
			break;
		case -2:
			m_direction = 1;
			break;
	}
}

void Interpreter::reverse()
{
	m_direction *= -1;
}

void Interpreter::string()
{
	if(!m_stringMode)
		m_stack->push('\0');

	m_stringMode = !m_stringMode;
}

void Interpreter::character()
{
	move();
	m_stack->push(QChar('\0').unicode());
	m_stack->push(m_space->getChar(m_pos).unicode());
}

void Interpreter::duplicate()
{
	int x = m_stack->pop();
	m_stack->push(x);
	m_stack->push(x);
}

void Interpreter::pop()
{
	m_stack->pop();
}

void Interpreter::swap()
{
	int a = m_stack->pop();
	int b = m_stack->pop();

	m_stack->push(a);
	m_stack->push(b);
}

void Interpreter::clear()
{
	m_stack->clear();
}

void Interpreter::vertIf()
{
	int x = m_stack->pop();
	if(x)
		up();
	else
		down();
}

void Interpreter::compare()
{
	int b = m_stack->pop();
	int a = m_stack->pop();

	if(a < b)
		turnLeft();
	else if(a > b)
		turnRight();
}

void Interpreter::printChar()
{
	outputChar = QChar(m_stack->pop());
	qDebug() << outputChar;
	emit(output(outputChar));
}

void Interpreter::printDec()
{
	outputChar = QString::number(m_stack->pop())[0];
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
	int x = m_stack->pop();

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
	int x = m_stack->pop();
	move();

	for(int i = 0; i < x; ++i)
		compute(m_space->getChar(m_pos));
}

void Interpreter::beginBlock()
{
	int x = qAbs(m_stack->pop());
	QStack<int>* newStack;

	int s = m_stack->size();
	if(s >= x)
	{
		newStack = new QStack<int>();
		foreach(int o, m_stack->mid(s-x))
		{
			newStack->push(o);
		}
	}
	else
	{
		newStack = new QStack<int>();
		foreach(int o, m_stack->mid(0))
			newStack->push(o);

		for(int i = 0; i < x - s; ++i)
			newStack->push(QChar(' ').unicode());
	}

	m_stack = newStack;
	m_stackStack.push(m_stack);
}

void Interpreter::endBlock()
{
	
}

void Interpreter::pushNumber(QChar n)
{
	m_stack->push(QString(n).toInt());
}

void Interpreter::panic(QString message)
{
	message = "PANIC!: " + message;
	qDebug() << m_pos[0] << m_pos[1];
	qFatal(message.toAscii());
}
