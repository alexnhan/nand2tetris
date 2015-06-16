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

	// Destructor
	~CodeWriter();

	// Methods
	void setFileName(string filename); // informs the code writer that the translation of a new VM file started
	void writeArithmetic(string command); // writes the assembly code that is the translation of the given arithmetic
	void writePushPop(VMcommand pushOrPop, string segment, int index); // writes assembly code that is the translation of the given push or pop command
	void close(void); // closes output file

private:
	string filename_;
	ofstream outf;
};

#endif
