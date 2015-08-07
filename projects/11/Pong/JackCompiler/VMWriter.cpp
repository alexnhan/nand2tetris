#include "VMWriter.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

VMWriter::VMWriter(string fileName)
{
	fout_.open(fileName.c_str());
}

void VMWriter::close(void)
{
	if(fout_.is_open())
		fout_.close();
}

void VMWriter::writePush(vm::Segment segment, int index)
{
	fout_ << "push " << vm::segment2String(segment) << " " << index << endl;
}

void VMWriter::writePop(vm::Segment segment, int index)
{
	fout_ << "pop " << vm::segment2String(segment) << " " << index << endl;
}

void VMWriter::writeArithmetic(vm::Command command)
{
	fout_ << vm::command2String(command) << endl;
}

void VMWriter::writeLabel(string label)
{
	fout_ << "label " << label << endl;
}

void VMWriter::writeGoto(string label)
{
	fout_ << "goto " << label << endl;
}

void VMWriter::writeIf(string label)
{
	fout_ << "if-goto " << label << endl;
}

void VMWriter::writeCall(string name, int nArgs)
{
	fout_ << "call " << name << " " << nArgs << endl;
}

void VMWriter::writeFunction(string name, int nLocals)
{
	fout_ << "function " << name << " " << nLocals << endl;
}

void VMWriter::writeReturn(void)
{
	fout_ << "return" << endl;
}