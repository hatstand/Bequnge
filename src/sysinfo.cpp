#include "sysinfo.h"

#include <QProcess>
#include <QTime>
#include <QDate>
#include <QDir>

void SysInfo::pushEnvVariables(QStack<int>* stack)
{
	// Null terminated env variables
	stack->push('\0');
	foreach(QString i, QProcess::systemEnvironment())
	{
		stack->push('\0');
		for(int j = i.size() - 1; j > -1; --j)
		{
			stack->push(i[j].unicode());
		}
	}
}

void SysInfo::pushCommandLineArgs(QStack<int>* stack)
{
	// Command line arguments
	// TODO
	stack->push('\0');
}

void SysInfo::pushStackSizes(QStack<int>* stack, const StackStack& stackStack, int currentStackSize)
{
	// Stack sizes
	QVectorIterator<QStack<int>*> it(stackStack);
	it.toBack();
	while(it.hasPrevious())
	{
		QStack<int>* s = it.previous();
		// Stack size has to be *before* this call
		if(s == stack)
			stack->push(currentStackSize);
		else
			stack->push(s->size());
	}
}

void SysInfo::pushStackStackSize(QStack<int>* stack, const StackStack& stackStack)
{
	// Stack Stack size
	stack->push(stackStack.size());
}

void SysInfo::pushTime(QStack<int>* stack)
{
	QTime time = QTime::currentTime();
	stack->push(time.hour() * 256 * 256 + time.minute() * 256 + time.second());
}

void SysInfo::pushDate(QStack<int>* stack)
{
	// Date
	QDate date = QDate::currentDate();
	stack->push((date.year() - 1900)*256*256 + date.month()*256 + date.day());
}

void SysInfo::pushGreatestPoint(QStack<int>* stack, const FungeSpace& space)
{
	// Greatest point
	for(uint i = space.dimensions()-1; i > -1; --i)
	{
		stack->push(space.getPositiveEdge(i) - space.getNegativeEdge(i));
	}
}

void SysInfo::pushLeastPoint(QStack<int>* stack, const FungeSpace& space)
{
	// Least point
	for(uint i = space.dimensions()-1; i > -1; --i)
	{
		stack->push(space.getNegativeEdge(i));
	}
}

void SysInfo::pushStorageOffset(QStack<int>* stack, const Interpreter::InstructionPointer& ip)
{
	// Storage Offset
	for(uint i = ip.m_storageOffset.count() - 1; i > -1; ++i)
	{
		stack->push(ip.m_storageOffset[i]);
	}
}

void SysInfo::pushDirection(QStack<int>* stack, const Interpreter::InstructionPointer& ip)
{
	// IP Delta
	for(uint i = ip.m_direction.count() - 1; i > -1; ++i)
	{
		stack->push(ip.m_direction[i]);
	}
}

void SysInfo::pushPosition(QStack<int>* stack, const Interpreter::InstructionPointer& ip)
{
	// IP position
	for(uint i = ip.m_pos.count() - 1; i > -1; ++i)
	{
		stack->push(ip.m_pos[i]);
	}
}

void SysInfo::pushTeam(QStack<int>* stack, const Interpreter::InstructionPointer& ip)
{
	// Team number for IP
	stack->push(1);
}

void SysInfo::pushUUID(QStack<int>* stack, const Interpreter::InstructionPointer& ip)
{
	// Unique ID for IP
	// TODO: do this properly
	stack->push(ip.m_id);
}

void SysInfo::pushDimensions(QStack<int>* stack, const FungeSpace& space)
{
	// Dimensions
	stack->push(space.dimensions());
}

void SysInfo::pushSeparator(QStack<int>* stack)
{
	// Path separator
	stack->push(QDir::separator().unicode());
}

void SysInfo::pushOperatingParadigm(QStack<int>* stack)
{
	// Operating paradigm
	// Unavailable
	stack->push(0);
}

void SysInfo::pushVersion(QStack<int>* stack)
{
	// version
	stack->push(1);
}

void SysInfo::pushHandprint(QStack<int>* stack)
{
	// handprint
	stack->push(42);
}

void SysInfo::pushBytesPerCell(QStack<int>* stack)
{
	// bytes per cell
	stack->push(sizeof(int));
}

void SysInfo::pushFlags(QStack<int>* stack)
{
	// Flags
	int ret = 0;
	// t implemented
	ret |= 0x01;
	stack->push(ret);
}

