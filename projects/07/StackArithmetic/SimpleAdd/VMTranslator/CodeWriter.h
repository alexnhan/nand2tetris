#ifndef _CODEWRITER_
#define _CODEWRITER_

#include <iostream>
#include <string>
#include <fstream>
#include "Parser.h"
using namespace std;

class CodeWriter
{
public:
	// Constructor
	CodeWriter(string filename); // Opens output file
	CodeWriter();

	// Destructor
	~CodeWriter();

	// Methods
	void setFileName(string filename); // informs the code writer that the translation of a new VM file started
	void writeArithmetic(string command); // writes the assembly code that is the translation of the given arithmetic
	void writePushPop(VMcommand pushOrPop, string segment, int index); // writes assembly code that is the translation of the given push or pop command
	void close(void); // closes output file

	// Helper methods
	void popD(void); // Pop top of stack to register D
	void popGPR(int regNum); // Pop the value stored in register D to GPR, RAM 13-15
	void decSP(void); // Decrements stack pointer and points register A to top of stack value
	void incSP(void); // Increments stack pointer and points register A to top of stack
	void setReturn(void); // Sets location to return to after a jump, stores it in R15
	void getReturn(void); // Jumps back to location stored in R15
	void init(void); // writes preset asm code for eq, lt, and gt jumps and where to initially start the code
	void push(string loc, int i); // pushes value of *(location+index) to top of stack
	void pop(string loc, int i); // pops top of stack value to location+index address
	
private:
	string filename_;
	ofstream outf;
	int labelnum_;
	int start_;
};

#endif