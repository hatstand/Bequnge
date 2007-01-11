#ifndef SYSINFO_H
#define SYSINFO_H

#include <QStack>

#include "stackstack.h"
#include "interpreter.h"

namespace SysInfo
{
	void pushEnvVariables(QStack<int>* stack);
	void pushCommandLineArgs(QStack<int>* stack);
	void pushStackSizes(QStack<int>* stack, const StackStack& stackStack, int);
	void pushStackStackSize(QStack<int>* stack, const StackStack& stackStack);
	void pushTime(QStack<int>* stack);
	void pushDate(QStack<int>* stack);
	void pushGreatestPoint(QStack<int>* stack, const FungeSpace& space);
	void pushLeastPoint(QStack<int>* stack, const FungeSpace& space);
	void pushStorageOffset(QStack<int>* stack, const Interpreter::InstructionPointer& ip);
	void pushDirection(QStack<int>* stack, const Interpreter::InstructionPointer& ip);
	void pushPosition(QStack<int>* stack, const Interpreter::InstructionPointer& ip);
	void pushTeam(QStack<int>* stack, const Interpreter::InstructionPointer& ip);
	void pushUUID(QStack<int>* stack, const Interpreter::InstructionPointer& ip);
	void pushDimensions(QStack<int>* stack, const FungeSpace& space);
	void pushSeparator(QStack<int>* stack);
	void pushOperatingParadigm(QStack<int>* stack);
	void pushVersion(QStack<int>* stack);
	void pushHandprint(QStack<int>* stack);
	void pushBytesPerCell(QStack<int>* stack);
	void pushFlags(QStack<int>* stack);
}


#endif
