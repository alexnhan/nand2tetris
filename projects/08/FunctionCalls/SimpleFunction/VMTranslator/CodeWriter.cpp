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
	writeInit();
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
	filename_ = filename;
	if(start_==0)
	{
		filename = filename + ".asm";
		outf.open(filename.c_str());
		writeInit();
	}
	if (!outf.is_open())
		cerr << "Could not open output file " << filename_ << ".\n";
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
		setReturn(15);
		popD();
		decSP();
		outf << "D=D-M" << endl << "M=0" << endl;
		incSP();
		outf << "@MAKETRUE" << endl << "D;JEQ" << endl << "(RETURN" << labelnum_ << ")" << endl;
		labelnum_++;
	}
	else if (command == "gt")
	{
		setReturn(15);
		popD();
		decSP();
		outf << "D=D-M" << endl << "M=0" << endl;
		incSP();
		outf << "@MAKETRUE" << endl << "D;JLT" << endl << "(RETURN" << labelnum_ << ")" << endl;
		labelnum_++;
	}
	else if (command == "lt")
	{
		setReturn(15);
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

void CodeWriter::setReturn(int regNum)
{
	assert(regNum >= 13 && regNum <= 15);
	outf << "@RETURN" << labelnum_ << endl << "D=A" << endl << "@R" << regNum << endl << "M=D" << endl;
}

void CodeWriter::getReturn(int regNum)
{
	assert(regNum >= 13 && regNum <= 15);
	outf << "@R" << regNum << endl << "A=M" << endl << "0;JMP" << endl;
}

void CodeWriter::writeInit(void)
{
	outf << "@256" << endl << "D=A" << endl << "@SP" << endl << "M=D" << endl;
	outf << "@START" << start_ << endl << "0;JMP" << endl;
	outf << "(MAKETRUE)" << endl;
	decSP();
	outf << "M=-1" << endl;
	incSP();
	getReturn(15);
	outf << "(START" << start_ << ")" << endl;
	//writeCall("Sys.init",0); // SimpleFunction does not use bootstrap code.
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

void CodeWriter::writeLabel(string label)
{
	outf << "(" << functionName_ << "$" << label << ")" << endl;
}

void CodeWriter::writeGoto(string label)
{
	if(label != functionName_) // labels within the function
	{
		outf << "@" << functionName_ << "$" << label << endl;
		outf << "0;JMP" << endl;
	}
	else // label is the functionName_, thus go to the function
	{
		outf << "@" << label << endl;
		outf << "0;JMP" << endl;
	}
}

void CodeWriter::writeIf(string label)
{
	popD();
	outf << "@" << functionName_ << "$" << label << endl;
	outf << "D;JNE" << endl; // jump if the stack's top value is not equal to 0
}

void CodeWriter::writeCall(string functionName, int numArgs)
{
	// Pushing return value from label declared below onto stack
	outf << "@RETURN" << labelnum_ << endl << "D=A" << endl << "@SP" << endl << "A=M" << endl << "M=D" << endl;
	incSP();
	// Pushing LCL of calling function onto stack
	outf << "@LCL" << endl << "D=M" << endl << "@SP" << endl << "A=M" << endl << "M=D" << endl;
	incSP();
	// Pushing ARG of calling function onto stack
	outf << "@ARG" << endl << "D=M" << endl << "@SP" << endl << "A=M" << endl << "M=D" << endl;
	incSP();
	// Pushing THIS of calling function onto stack
	outf << "@THIS" << endl << "D=M" << endl << "@SP" << endl <<"A=M" << endl << "M=D" << endl;
	incSP();
	// Pushing THAT of calling function onto stack
	outf << "@THAT" << endl << "D=M" << endl << "@SP" << endl <<"A=M" << endl << "M=D" << endl;
	incSP();
	// repositioning ARG
	outf << "@" << numArgs << endl << "D=A" << endl << "@SP" << endl << "D=M-D" << endl << "@5" << endl << "D=D-A" << endl << "@ARG" << endl << "M=D" << endl;
	// repositioning LCL
	outf << "@SP" << endl << "D=M" << endl << "@LCL" << endl << "M=D" << endl;
	// transfer control
	outf << "@" << functionName << endl << "0;JMP" << endl;
	// declare label for return-address
	outf << "(RETURN" << labelnum_ << ")" << endl;
	labelnum_++;
}

void CodeWriter::writeReturn(void)
{
	// FRAME is a temporary variable
	outf << "@LCL" << endl << "D=M" << endl << "@FRAME" << endl << "M=D" << endl;
	// Put the return-address in a temp variable
	outf << "@5" << endl << "D=A" << endl << "@FRAME" << endl << "A=M-D" << endl << "D=M" << endl << "@RET" << endl << "M=D" << endl;
	// Reposition the return value for the caller
	popD(); // pop return value to register D
	outf << "@ARG" << endl << "A=M" << endl << "M=D" << endl;
	// Restore SP of the caller
	outf << "@ARG" << endl << "D=M+1" << endl << "@SP" << endl << "M=D" << endl;
	// Restore THAT of the caller
	outf << "@FRAME" << endl << "A=M-1" << endl << "D=M" << endl << "@THAT" << endl << "M=D" << endl;
	// Restore THIS of the caller
	outf << "@2" << endl << "D=A" << endl << "@FRAME" << endl << "A=M-D" << endl << "D=M" << endl << "@THIS" << endl << "M=D" << endl;
	// Restore ARG of the caller
	outf << "@3" << endl << "D=A" << endl << "@FRAME" << endl << "A=M-D" << endl << "D=M" << endl << "@ARG" << endl << "M=D" << endl;
	// Restore LCL of the caller
	outf << "@4" << endl << "D=A" << endl << "@FRAME" << endl << "A=M-D" << endl << "D=M" << endl << "@LCL" << endl << "M=D" << endl;
	// go to return-address
	outf << "@RET" << endl << "A=M" << endl << "0;JMP" << endl;
}

void CodeWriter::writeFunction(string functionName, int numLocals)
{
	functionName_=functionName;
	// Declare a label for the function entry
	outf << "(" << functionName << ")" << endl;
	// initialize all local variables to 0
	for(int i=0;i<numLocals;i++)
		writePushPop(C_PUSH,"constant",0);
}

void CodeWriter::setOutputFileName(string outname)
{
	filename_=outname;
	outname = outname+".asm";
	outf.open(outname.c_str());
	writeInit();
	start_++;
}