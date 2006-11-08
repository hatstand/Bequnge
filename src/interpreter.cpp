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
	m_counter = &zeroth;
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

	while((line = m_input->readLine()) != NULL)
	{
		line.chop(1);
		// zeroth is the 0th dimension
		// This represents the 1st dimension
		Array* v = new Array();
		for(int i = 0; i < line.length(); ++i)
		{
			v->append(new Array(line[i]));
		}
		zeroth.append(v);
	}

	qDebug() << "Finished reading code";
}

void Interpreter::run()
{
	// Get the first Array that actually contains a character
	if(!(m_counter->isChar()))
	{
		m_counter = m_counter->getNext();
	}

	qDebug() << "Counter: " << m_counter->toString();

	// Do instruction
	compute(m_counter);

	// Calculate next position for counter
	m_counter = m_counter->parent;
}

// Call this with a QChar Array
void Interpreter::compute(Array* ptr)
{
	if(!ptr->isChar())
		qFatal("Compute: Array is not a character");

	QChar command = ptr->getContents();

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
	else
		panic();
}

const QChar Interpreter::getChar(Array* a)
{
	if(a->isChar())
		return a->getContents();
	else
		return getChar(a->getNext());
}

void Interpreter::parse()
{
	parseHeader();

	readInAll();

	qDebug() << zeroth.toString();
}


//Instructions
void Interpreter::add()
{
	ushort x = QString(m_stack.pop()).toUShort();
	ushort y = QString(m_stack.pop()).toUShort();

	ushort z = y - x;

	qDebug() << x << "+" << y << "=" << z;
	m_stack.push(QChar(z));
}

void Interpreter::subtract()
{

}

void Interpreter::multiply()
{

}

void Interpreter::divide()
{

}

void Interpreter::modulo()
{

}

void Interpreter::notf()
{

}

void Interpreter::greaterThan()
{

}

void Interpreter::pushNumber(QChar n)
{
	m_stack.push(n);
}

void Interpreter::panic()
{
	qFatal("PANIC!");
}
