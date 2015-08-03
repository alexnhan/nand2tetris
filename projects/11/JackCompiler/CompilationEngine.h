#ifndef _COMPILATIONENGINE_
#define _COMPILATIONENGINE_

#include "JackTokenizer.h"
#include "SymbolTable.h"
#include "VMWriter.h"
#include <iostream>
//#include <fstream>
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
	void compileSubroutine(void); // Compiles a complete method, function, or constructor
	void compileParameterList(void); // Compiles a (possibly empty) parameter list, not including the enclosing "()"
	void compileSubroutineBody(void); // Compiles subroutine body
	void compileVarDec(void); // Compiles a var declaration
	void compileStatements(void); // Compiles a sequence of statements, not including the enclosing {}.
	void compileDo(void); // Compiles a do statement
	void compileLet(void); // Compiles a let statement
	void compileWhile(void); // Compiles a while statement
	void compileReturn(void); // Compiles a return statement
	void compileIf(void); // Compiles an if statement, possible with a trailing else clause
	void compileExpression(void); // Compiles an expression
	void compileTerm(void); // Compiles a term.  Uses a single lookahead
	void compileExpressionList(void); // Compiles a (possibly empty) comma-separated list of expressions
	
private:
	JackTokenizer * JT_;
	SymbolTable ST_;
	VMWriter VM_;
	string currentFunctionName_;
	string className_;
};

#endif