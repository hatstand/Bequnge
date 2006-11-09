#include "interpreter.h"

#include <QDebug>
#include <QStringList>

Interpreter::Interpreter(QIODevice* input, QObject* parent)
	: m_input(input)
{
	m_version = "1";
	m_dimensions = 2;
	m_direction = 1;
	m_pos[0] = 0;
	m_pos[1] = 0;

	m_edgeLeft = 0;
	m_edgeRight = 0;

	m_stringMode = false;
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

	qDebug() << "Finished parsing header";
}

void Interpreter::readInAll()
{
	qDebug() << "Reading in code";
	QString line;

	FungeSpace<2>::Coord pos;
	pos[0] = 0;
	pos[1] = 0;

	while((line = m_input->readLine()) != NULL)
	{
		int i = 0;
		for(; i < line.length(); ++i)
		{
			if(line[i] == '\n')
				break;

			pos[0] = i;
			//qDebug() << "Putting:" << line[i] << "in:" << pos[0] << pos[1];
			m_space.setChar(pos, line[i]);
		}

		// TODO: Dimension independent edge detection
		m_edgeRight = qMax(m_edgeRight, i);
		// TODO: left edge detection
		m_edgeLeft = 0;
		
		++pos[1];
	}
	m_edgeRight--;

	m_edgeTop = 0;
	m_edgeBottom = pos[1];
	qDebug() << "Edge:" << m_edgeRight;

	qDebug() << "Finished reading code";
}

void Interpreter::jumpSpaces()
{
	if(m_space.getChar(m_pos).category() == QChar::Separator_Space)
	{
		m_jumpedSpace = true;	
		move();
	}
}

void Interpreter::move()
{
	switch(m_direction)
	{
		case 1:
			++m_pos[0];
			break;
		case 2:
			++m_pos[1];
			break;
		case -1:
			--m_pos[0];
			break;
		case -2:
			--m_pos[1];
			break;
		default:
			panic();
	}

	//qDebug() << "Moved to:" << m_pos[0] << m_pos[1] << m_space.getChar(m_pos);

	if(m_pos[0] < m_edgeLeft)
	{
		m_pos[0] = m_edgeRight;
	}
	else if(m_pos[0] > m_edgeRight)
	{
		m_pos[0] = m_edgeLeft;
	}
	else if(m_pos[1] < m_edgeTop)
	{
		m_pos[1] = m_edgeBottom;
	}
	else if(m_pos[1] > m_edgeBottom)
	{
		m_pos[1] = m_edgeTop;
	}

	jumpSpaces();
}

bool Interpreter::step()
{
	if(m_jumpedSpace)
	{
		m_jumpedSpace = false;
		if(m_stringMode)
			m_stack.push(QChar(' ').unicode());
	}
	QChar c = m_space.getChar(m_pos);
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
		m_stack.push(command.unicode());
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
	else if(command == ':')
		duplicate();
	else if(command == '|')
		vertIf();
	else if(command == ',')
		printChar();
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
	int x = m_stack.pop();
	int y = m_stack.pop();

	int z = y + x;

	qDebug() << x << "+" << y << "=" << z;
	m_stack.push(z);
}

void Interpreter::subtract()
{
	int x = m_stack.pop();
	int y = m_stack.pop();

	int z = y - x;

	qDebug() << y << "-" << x << "=" << z;
	m_stack.push(z);
}

void Interpreter::multiply()
{
	int x = m_stack.pop();
	int y = m_stack.pop();

	int z = y * x;

	qDebug() << y << "*" << x << "=" << z;
	m_stack.push(z);
}

void Interpreter::divide()
{
	int x = m_stack.pop();
	int y = m_stack.pop();

	int z = y * x;

	qDebug() << y << "/" << x << "=" << z;
	m_stack.push(z);
}

void Interpreter::modulo()
{
	int x = m_stack.pop();
	int y = m_stack.pop();

	int z = y % x;

	qDebug() << y << "%" << x << "=" << z;
	m_stack.push(z);
}

void Interpreter::notf()
{
	int x = m_stack.pop();
	if(x)
		m_stack.push(1);
	else
		m_stack.push(0);
}

void Interpreter::greaterThan()
{
	int x = m_stack.pop();
	int y = m_stack.pop();

	if(y > x)
		m_stack.push(1);
	else
		m_stack.push(0);
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
		m_stack.push('\0');

	m_stringMode = !m_stringMode;
}

void Interpreter::duplicate()
{
	int x = m_stack.pop();
	m_stack.push(x);
	m_stack.push(x);
}

void Interpreter::vertIf()
{
	int x = m_stack.pop();
	if(x)
		up();
	else
		down();
}

void Interpreter::printChar()
{
	qWarning() << QChar(m_stack.pop());
}

void Interpreter::pushNumber(QChar n)
{
	m_stack.push(QString(n).toInt());
}

void Interpreter::panic(QString message)
{
	message = "PANIC!: " + message;
	qDebug() << m_pos[0] << m_pos[1];
	qFatal(message.toAscii());
}
