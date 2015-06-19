#include <iostream>
#include <string>
#include "Parser.h"
#include "CodeWriter.h"
#include "Parser.cpp"
#include "CodeWriter.cpp"
using namespace std;

int main(int argc, char *argv[])
{
	CodeWriter asm1;
	for(int i=1;i<argc;i++)
	{
		string filename = argv[i];
		int dot = filename.find(".",0);
		filename = filename.substr(0,dot);
		Parser vm1(filename);
		asm1.setFileName(filename);
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
}