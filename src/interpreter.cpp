#include "interpreter.h"

#include <QDebug>
#include <QStringList>

Interpreter::Interpreter(QIODevice* input, QObject* parent)
	: m_input(input)
{
	m_input->open(QIODevice::ReadOnly);
	m_version = "1";
	m_dimensions = 2;
	m_direction = 1;
	m_pos[0] = 0;
	m_pos[1] = 0;
}

Interpreter::~Interpreter()
{
	m_input->close();
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

	int pos[2] = {0,0};

	while((line = m_input->readLine()) != NULL)
	{
		line.chop(1);

		for(int i = 0; i < line.length(); ++i)
		{
			pos[0] = i;
			m_space.setChar(pos, line[i]);
		}
		
		++pos[1];
	}

	qDebug() << "Finished reading code";
}

bool Interpreter::step()
{
	QChar c = m_space.getChar(m_pos);
	bool ret = compute(c);

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
	else if(command.isNumber())
		pushNumber(command);
	else if(command == '@')
		return false;
	else
		panic();

	return true;
}

void Interpreter::parse()
{
	parseHeader();

	readInAll();
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

void Interpreter::pushNumber(QChar n)
{
	m_stack.push(QString(n).toInt());
}

void Interpreter::panic()
{
	qFatal("PANIC!");
}
