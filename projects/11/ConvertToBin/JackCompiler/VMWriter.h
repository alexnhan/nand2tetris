#ifndef _VMWRITER_
#define _VMWRITER_

#include <string>
#include <fstream>
using namespace std;

namespace vmwriter
{
	enum Segment { CONST, ARG, LOCAL, STATIC, THIS, THAT, POINTER, TEMP };
	enum Command { ADD, SUB, NEG, EQ, GT, LT, AND, OR, NOT };
	
	string segment2String(Segment seg)
	{
		switch(seg)
		{
			case CONST:
				return "constant";
			case ARG:
				return "argument";
			case LOCAL:
				return "local";
			case STATIC:
				return "static";
			case THIS:
				return "this";
			case THAT:
				return "that";
			case POINTER:
				return "pointer";
			case TEMP:
				return "temp";
		}
	}
	
	string command2String(Command com)
	{
		switch(com)
		{
			case ADD:
				return "add";
			case SUB:
				return "sub";
			case NEG:
				return "neg";
			case EQ:
				return "eq";
			case GT:
				return "gt";
			case LT:
				return "lt";
			case AND:
				return "and";
			case OR:
				return "or";
			case NOT:
				return "not";
		}
	}
};
namespace vm=vmwriter;


class VMWriter
{
public:
	// Constructor
	VMWriter(string fileName); // Creates a new file and prepares it for writing.
	
	// Methods
	void writePush(vm::Segment segment, int index); // Writes a VM push command
	void writePop(vm::Segment segment, int index); // Writes a VM pop command
	void writeArithmetic(vm::Command command); // Writes a VM arithmetic command
	void writeLabel(string label); // Writes a VM label comand
	void writeGoto(string label); // Writes a VM goto command
	void writeIf(string label); // Writes a VM if-goto command
	void writeCall(string name, int nArgs); // Writes a VM call command
	void writeFunction(string name, int nLocals); // Writes a VM function command
	void writeReturn(void); // Writes a VM return command
	void close(void); // Closes the output file
	
private:
	ofstream fout_;
};

#endif