#ifndef _PARSER_
#define _PARSER_

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

// VM commands
enum VMcommand { C_ARITHMETIC, C_PUSH, C_POP, C_LABEL, C_GOTO, C_IF, C_FUNCTION, C_RETURN, C_CALL };

class Parser
{
public:
	// Constructor
	Parser(string filename);

	// Destructor
	~Parser();

	// Methods
	bool hasMoreCommands(void); // are there more commands in the input?
	void advance(void); // reads next command from input, should only be called if hasMoreCommands() is true
	VMcommand commandType(void); // returns the type of the current VM command
	string arg1(void); // returns the first argument of the current command
	int arg2(void); // returns the second argument of the current command

private: // Attributes
	string filename_;
	string currentCommand_;
	ifstream inf;
};

#endif