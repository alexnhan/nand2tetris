#ifndef _COMPILATIONENGINE_
#define _COMPILATIONENGINE_

#include "JackTokenizer.h"
#include <iostream>
#include <fstream>
using namespace std;

class CompilationEngine
{
public:
	// Constructor
	CompilationEngine(JackTokenizer * JT, string outputFile); // Creates a new compilation engine with the given input and out. The next routine called must be compileClass()
	// Destructor
	~CompilationEngine();
	
	// Methods
	void compileClass(void); // Compiles a complete class
	void compileClassVarDec(void); // Compiles a static declaration or a field declaration
	
private:
	JackTokenizer * JT_;
	ofstream fout_;
};

#endif