#include "CodeWriter.h"
#include <string>
#include <iostream>
#include <fstream>
#include <assert.h>
using namespace std;

// Constructor
CodeWriter::CodeWriter()
{
	labelnum_ = 0;
	start_=0;
}
CodeWriter::CodeWriter(string filename)
{
	labelnum_ = 0;
	start_=0;
	filename_ = filename;
	filename = filename + ".asm";
	outf.open(filename.c_str());
	if (!outf.is_open())
		cerr << "Could not open output file " << filename_ << ".\n";
	init();
}

// Destructor
CodeWriter::~CodeWriter()
{
	if (outf.is_open())
		outf.close();
}

// Methods
void CodeWriter::setFileName(string filename)
{
	if (outf.is_open())
		outf.close();
	filename_ = filename;
	filename = filename + ".asm";
	outf.open(filename.c_str());
	if (!outf.is_open())
		cerr << "Could not open output file " << filename_ << ".\n";
	init();
}

void CodeWriter::writeArithmetic(string command)
{
	if (command == "add")
	{
		popD();
		decSP();
		outf << "M=D+M" << endl;
		incSP();
	}
	else if (command == "sub")
	{
		popD();
		decSP();
		outf << "M=M-D" << endl;
		incSP();
	}
	else if (command == "neg")
	{
		decSP();
		outf << "M=-M" << endl;
		incSP();
	}
	else if (command == "not")
	{
		decSP();
		outf << "M=!M" << endl;
		incSP();
	}
	else if (command == "and")
	{
		popD();
		decSP();
		outf << "M=D&M" << endl;
		incSP();
	}
	else if (command == "or")
	{
		popD();
		decSP();
		outf << "M=D|M" << endl;
		incSP();
	}
	else if (command == "eq")
	{
		setReturn();
		popD();
		decSP();
		outf << "D=D-M" << endl << "M=0" << endl;
		incSP();
		outf << "@MAKETRUE" << endl << "D;JEQ" << endl << "(RETURN" << labelnum_ << ")" << endl;
		labelnum_++;
	}
	else if (command == "gt")
	{
		setReturn();
		popD();
		decSP();
		outf << "D=D-M" << endl << "M=0" << endl;
		incSP();
		outf << "@MAKETRUE" << endl << "D;JLT" << endl << "(RETURN" << labelnum_ << ")" << endl;
		labelnum_++;
	}
	else if (command == "lt")
	{
		setReturn();
		popD();
		decSP();
		outf << "D=D-M" << endl << "M=0" << endl;
		incSP();
		outf << "@MAKETRUE" << endl << "D;JGT" << endl << "(RETURN" << labelnum_ << ")" << endl;
		labelnum_++;
	}
}

void CodeWriter::writePushPop(VMcommand pushOrPop, string segment, int index)
{
	if (pushOrPop == C_PUSH)
	{
		if (segment == "constant")
		{
			outf << "@" << index << endl << "D=A" << endl << "@SP" << endl << "A=M" << endl << "M=D" << endl;
			incSP();
		}
		else if (segment == "local")
		{
			push("LCL",index);
		}
		else if (segment == "argument")
		{
			push("ARG",index);
		}
		else if (segment == "this")
		{
			push("THIS",index);
		}
		else if (segment == "that")
		{
			push("THAT",index);
		}
		else if (segment == "pointer")
		{
			push("3",index);
		}
		else if (segment == "temp")
		{
			push("5",index);
		}
		else if (segment == "static")
		{
			outf << "@" << filename_ << "." << index << endl << "D=M" << endl << "@SP" << endl << "A=M" << endl << "M=D" << endl << "@SP" << endl << "M=M+1" << endl;
		}
	}
	else if (pushOrPop == C_POP)
	{
		if (segment == "local")
		{
			pop("LCL",index);
		}
		else if (segment == "argument")
		{
			pop("ARG",index);
		}
		else if (segment == "this")
		{
			pop("THIS",index);
		}
		else if (segment == "that")
		{
			pop("THAT",index);
		}
		else if (segment == "pointer")
		{
			pop("3",index);
		}
		else if (segment == "temp")
		{
			pop("5",index);
		}
		else if (segment == "static")
		{
			outf << "@" << filename_ << "." << index << endl << "D=A" << endl << "@R13" << endl << "M=D" << endl << "@SP" << endl << "M=M-1" << endl << "A=M" << endl << "D=M" << endl << "@R13" <<
				endl << "A=M" << endl << "M=D" << endl;
		}
	}
}

void CodeWriter::popD(void)
{
	outf << "@SP" << endl << "AM=M-1" << endl << "D=M" << endl;
}

void CodeWriter::popGPR(int regNum)
{
	assert(regNum >= 13 && regNum <= 15);
	outf << "@" << regNum << endl << "M=D" << endl;
}

void CodeWriter::decSP(void)
{
	outf << "@SP" << endl << "AM=M-1" << endl;
}

void CodeWriter::incSP(void)
{
	outf << "@SP" << endl << "AM=M+1" << endl;
}

void CodeWriter::setReturn(void)
{
	outf << "@RETURN" << labelnum_ << endl << "D=A" << endl << "@R15" << endl << "M=D" << endl;
}

void CodeWriter::getReturn(void)
{
	outf << "@R15" << endl << "A=M" << endl << "0;JMP" << endl;
}

void CodeWriter::init(void)
{
	outf << "@START" << start_ << endl << "0;JMP" << endl << "(MAKETRUE)" << endl;
	decSP();
	outf << "M=-1" << endl;
	incSP();
	getReturn();
	outf << "(START" << start_ << ")" << endl;
	start_++;
}

void CodeWriter::push(string loc, int i)
{
	if(loc == "3" || loc == "5")
	{
		outf << "@" << loc << endl << "D=A" << endl << "@" << i << endl << "A=D+A" << endl << "D=M" << endl << "@SP" << endl << "A=M" << endl << "M=D" << endl;
		incSP();
	}
	else
	{
		outf << "@" << loc << endl << "D=M" << endl << "@" << i << endl << "A=D+A" << endl << "D=M" << endl << "@SP" << endl << "A=M" << endl << "M=D" << endl;
		incSP();
	}
}

void CodeWriter::pop(string loc, int i)
{
	if(loc == "3" || loc == "5")
	{
		outf << "@" << loc << endl << "D=A" << endl << "@" << i << endl << "D=D+A" << endl;
		popGPR(13);
		decSP();
		outf << "D=M" << endl << "@R13" << endl << "A=M" << endl << "M=D" << endl;
	}
	else
	{
		outf << "@" << loc << endl << "D=M" << endl << "@" << i << endl << "D=D+A" << endl;
		popGPR(13);
		decSP();
		outf << "D=M" << endl << "@R13" << endl << "A=M" << endl << "M=D" << endl;
	}
}