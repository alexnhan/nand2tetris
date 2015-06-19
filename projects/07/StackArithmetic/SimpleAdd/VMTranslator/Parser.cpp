#include "Parser.h"
#include <fstream>
#include <string>
#include <iostream>
using namespace std;

// Constructor
Parser::Parser(string filename)
{
	filename_ = filename;
	filename = filename + ".vm";
	inf.open(filename.c_str());
	if (!inf.is_open())
		cerr << "Error: Could not open " << filename << ".\n";
}

// Destructor
Parser::~Parser()
{
	if (inf.is_open())
		inf.close();
}

// Methods
bool Parser::hasMoreCommands(void)
{
	return !inf.eof();
}

void Parser::advance(void)
{
	if (hasMoreCommands())
	{
		getline(inf, currentCommand_);
		while (((currentCommand_.substr(0, 2) == "//") || (currentCommand_.substr(0, 1) == "")) && hasMoreCommands())
			getline(inf, currentCommand_);
	}
}

VMcommand Parser::commandType(void)
{
	if ((currentCommand_.substr(0, 3) == "add") || (currentCommand_.substr(0, 3) == "sub") || (currentCommand_.substr(0, 3) == "neg") || (currentCommand_.substr(0, 3) == "and") || (currentCommand_.substr(0, 3) == "not") || (currentCommand_.substr(0, 2) == "eq") || (currentCommand_.substr(0, 2) == "gt") || (currentCommand_.substr(0, 2) == "lt") || (currentCommand_.substr(0, 2) == "or"))
		return C_ARITHMETIC;
	else if ((currentCommand_.substr(0, 4) == "push"))
		return C_PUSH;
	else if ((currentCommand_.substr(0, 3) == "pop"))
		return C_POP;
	else if ((currentCommand_.substr(0, 5) == "label"))
		return C_LABEL;
	else if ((currentCommand_.substr(0, 4) == "goto"))
		return C_GOTO;
	else if ((currentCommand_.substr(0, 2) == "if"))
		return C_IF;
	else if ((currentCommand_.substr(0, 8) == "function"))
		return C_FUNCTION;
	else if ((currentCommand_.substr(0, 6) == "return"))
		return C_RETURN;
	else if ((currentCommand_.substr(0, 4) == "call"))
		return C_CALL;
}

string Parser::arg1(void)
{
	VMcommand ct = commandType();
	if (ct != C_RETURN)
	{
		int firstSpace = currentCommand_.find(" ", 0);
		int secondSpace = currentCommand_.find(" ", firstSpace + 1);
		string a1 = currentCommand_.substr(firstSpace + 1, secondSpace - firstSpace - 1);
		return a1;
	}
}

int Parser::arg2(void)
{
	VMcommand ct = commandType();
	if (ct == C_PUSH || ct == C_POP || ct == C_FUNCTION || ct == C_CALL)
	{
		int firstSpace = currentCommand_.find(" ", 0);
		int secondSpace = currentCommand_.find(" ", firstSpace + 1);
		int thirdSpace = currentCommand_.find(" ", secondSpace + 1);
		string a2 = currentCommand_.substr(secondSpace + 1, thirdSpace - secondSpace - 1);
		int a2i = stoi(a2);
		return a2i;
	}
}