#ifndef SYSINFO_H
#define SYSINFO_H

#include <QStack>

#include "stackstack.h"
#include "interpreter.h"

namespace SysInfo
{
	void pushEnvVariables(Stack* stack);
	void pushCommandLineArgs(Stack* stack);
	void pushStackSizes(Stack* stack, StackStack* stackStack, int);
	void pushStackStackSize(Stack* stack, StackStack* stackStack);
	void pushTime(Stack* stack);
	void pushDate(Stack* stack);
	void pushGreatestPoint(Stack* stack, const FungeSpace& space);
	void pushLeastPoint(Stack* stack, const FungeSpace& space);
	void pushStorageOffset(Stack* stack, const Interpreter::InstructionPointer& ip);
	void pushDirection(Stack* stack, const Interpreter::InstructionPointer& ip);
	void pushPosition(Stack* stack, const Interpreter::InstructionPointer& ip);
	void pushTeam(Stack* stack, const Interpreter::InstructionPointer& ip);
	void pushUUID(Stack* stack, const Interpreter::InstructionPointer& ip);
	void pushDimensions(Stack* stack, const FungeSpace& space);
	void pushSeparator(Stack* stack);
	void pushOperatingParadigm(Stack* stack);
	void pushVersion(Stack* stack);
	void pushHandprint(Stack* stack);
	void pushBytesPerCell(Stack* stack);
	void pushFlags(Stack* stack);
}


#endif
