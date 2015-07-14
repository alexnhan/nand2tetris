#ifndef _COMPILATIONENGINE_
#define _COMPILATIONENGINE_

#include "JackTokenizer.h"
#include "JackTokenizer.cpp"
#include <iostream>
#include <fstream>
using namespace std;

class CompilationEngine
{
public:
	// Constructor
	CompilationEngine(JackTokenizer * JT, ofstream & fout); // Creates a new compilation engine with the given input and out. The next routine called must be compileClass()
	
private:
	JackTokenizer * JT_;
	ofstream fout_;
};

#endif