#include "sysinfo.h"

#include <QProcess>
#include <QTime>
#include <QDate>
#include <QDir>

void SysInfo::pushEnvVariables(Stack* stack)
{
	// Null terminated env variables
	stack->push(0);
	foreach(QString i, QProcess::systemEnvironment())
	{
		stack->push(0);
		for(int j = i.size() - 1; j > -1; --j)
		{
			stack->push(i[j].unicode());
		}
	}
}

void SysInfo::pushCommandLineArgs(Stack* stack)
{
	// Command line arguments
	// TODO
	stack->push(0);
	stack->push(0);
}

void SysInfo::pushStackSizes(Stack* stack, StackStack* stackStack, int currentStackSize)
{
	// Stack sizes
	QStack<int> tempStack;
	for (int i=0 ; i<stackStack->count() ; ++i)
	{
		Stack* s = (Stack*) stackStack->child(i);
		if (stack == s)
			tempStack.push(currentStackSize);
		else
			tempStack.push(s->count());
	}
	while (tempStack.count() > 0)
		stack->push(tempStack.pop());
}

void SysInfo::pushStackStackSize(Stack* stack, StackStack* stackStack)
{
	// Stack Stack size
	stack->push(stackStack->count());
}

void SysInfo::pushTime(Stack* stack)
{
	QTime time = QTime::currentTime();
	stack->push(time.hour() * 256 * 256 + time.minute() * 256 + time.second());
}

void SysInfo::pushDate(Stack* stack)
{
	// Date
	QDate date = QDate::currentDate();
	stack->push((date.year() - 1900)*256*256 + date.month()*256 + date.day());
}

void SysInfo::pushGreatestPoint(Stack* stack, const FungeSpace& space)
{
	// Greatest point
	for(uint i = 0; i < space.dimensions(); ++i)
		stack->push(space.getPositiveEdge(i) - space.getNegativeEdge(i));
}

void SysInfo::pushLeastPoint(Stack* stack, const FungeSpace& space)
{
	// Least point
	for(uint i = 0; i < space.dimensions(); ++i)
		stack->push(space.getNegativeEdge(i));
}

void SysInfo::pushStorageOffset(Stack* stack, const Interpreter::InstructionPointer& ip)
{
	// Storage Offset
	for (uint i = 0; i < ip.m_storageOffset.count(); ++i)
		stack->push(ip.m_storageOffset.at(i));
}

void SysInfo::pushDirection(Stack* stack, const Interpreter::InstructionPointer& ip)
{
	// IP Delta
	for (uint i = 0; i < ip.m_direction.count(); ++i)
		stack->push(ip.m_direction.at(i));
}

void SysInfo::pushPosition(Stack* stack, const Interpreter::InstructionPointer& ip)
{
	// IP position
	for (uint i = 0; i < ip.m_pos.count(); ++i)
		stack->push(ip.m_pos.at(i));
}

void SysInfo::pushTeam(Stack* stack, const Interpreter::InstructionPointer& ip)
{
	// Team number for IP
	stack->push(1);
}

void SysInfo::pushUUID(Stack* stack, const Interpreter::InstructionPointer& ip)
{
	// Unique ID for IP
	// TODO: do this properly
	stack->push(ip.m_id);
}

void SysInfo::pushDimensions(Stack* stack, const FungeSpace& space)
{
	// Dimensions
	stack->push(space.dimensions());
}

void SysInfo::pushSeparator(Stack* stack)
{
	// Path separator
	stack->push(QDir::separator().unicode());
}

void SysInfo::pushOperatingParadigm(Stack* stack)
{
	// Operating paradigm
	// Unavailable
	stack->push(0);
}

void SysInfo::pushVersion(Stack* stack)
{
	// version
	stack->push(1);
}

void SysInfo::pushHandprint(Stack* stack)
{
	// handprint
	stack->push(42);
}

void SysInfo::pushBytesPerCell(Stack* stack)
{
	// bytes per cell
	stack->push(sizeof(int));
}

void SysInfo::pushFlags(Stack* stack)
{
	// Flags
	int ret = 0;
	// t implemented
	ret |= 0x01;
	stack->push(ret);
}

