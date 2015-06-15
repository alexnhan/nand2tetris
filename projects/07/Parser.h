#ifndef _PARSER_
#define _PARSER_

#include <fstream>
#include <string>

class Parser
{
	public:
		// VM commands
		enum VMcommand { C_ARITHMETIC, C_PUSH, C_POP, C_LABEL, C_GOTO, C_IF, C_FUNCTION, C_RETURN, C_CALL };

		// Constructor
		Parser(string filename);

		// Destructor
		~Parser();

		// Methods
		bool hasMoreCommands() const; // are there more commands in the input?
		void advance(); // reads next command from input, should only be called if hasMoreCommands() is true
		VMcommand commandType(); // returns the type of the current VM command
		string arg1(); // returns the first argument of the current command
		int arg2(); // returns the second argument of the current command

	private: // Attributes
		string filename_;
		string currentCommand_;
		ifstream inf;
};

#endif
