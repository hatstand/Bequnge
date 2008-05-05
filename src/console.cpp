#include "console.h"

Console::Console(Interpreter* i) : QObject(i), m_in(stdin), m_out(stdout)
{
	connect(i, SIGNAL(output(QChar)), SLOT(writeOutput(QChar)));
	connect(i, SIGNAL(output(QString)), SLOT(writeOutput(QString)));
	connect(i, SIGNAL(input(Interpreter::WaitingForInput)), SLOT(readInput(Interpreter::WaitingForInput)));
}

Console::~Console()
{
}

void Console::writeOutput(QChar c)
{
	m_out << c;
	m_out.flush();
}

void Console::writeOutput(QString s)
{
	m_out << s;
	m_out.flush();
}

void Console::readInput(Interpreter::WaitingForInput type)
{
	qDebug() << "Waiting for input:" << (type == Interpreter::WaitingForChar ? "char" : "int");
	if (type == Interpreter::WaitingForChar || type == Interpreter::WaitingForInteger)
	{
		QString s = m_in.readLine();
		while (s.length() > 0)
		{
			if (type == Interpreter::WaitingForChar)
			{
				QChar c = s[0];
				m_interpreter->provideInput(c);
				return;
			}
			else
			{
				bool ok = false;
				int x = s.toInt(&ok);
				if (ok)
				{
					m_interpreter->provideInput(x);
					return;
				}
			}

			s = m_in.readLine();
		}
	}
}
