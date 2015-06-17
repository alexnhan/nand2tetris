#include <iostream>
#include <string>
#include "Parser.h"
#include "CodeWriter.h"
using namespace std;

int main(int argc, char *argv[])
{
	string filename = "StackTest";
	Parser vm1(filename);
	CodeWriter asm1(filename);
	while (vm1.hasMoreCommands())
	{
		vm1.advance();
		if (vm1.commandType() == C_ARITHMETIC)
		{
			asm1.writeArithmetic(vm1.arg1());
		}
		else if (vm1.commandType() == C_POP)
		{
			asm1.writePushPop(C_POP, vm1.arg1(), vm1.arg2());
		}
		else if (vm1.commandType() == C_PUSH)
		{
			asm1.writePushPop(C_PUSH, vm1.arg1(), vm1.arg2());
		}
	}
}
