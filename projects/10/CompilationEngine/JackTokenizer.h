#ifndef _JACKTOKENIZER_
#define _JACKTOKENIZER_

#include <fstream>
#include <string>
using namespace std;

// Token Types
enum tokentype { KEYWORD, SYMBOL, IDENTIFIER, INT_CONST, STRING_CONST };

// keyWord
enum keyword { CLASS, METHOD, FUNCTION, CONSTRUCTOR, INT, BOOLEAN, CHAR, VOID, VAR, STATIC, FIELD, LET, DO, IF, ELSE, WHILE, RETURN, TRUE, FALSE, null, THIS };

class JackTokenizer
{
public:
	// Constructor
	JackTokenizer(string inputFile); // Opens the input file and gets ready to tokenize it
	
	// Methods
	bool hasMoreTokens(void); // Do we have more tokens in the input?
	void advance(void); // Gets the next token from the input and makes it the current token. This method is only called if hasMoreTokens is true. Initially there is no current token.
	tokentype tokenType(void); // Returns the type of the current token
	keyword keyWord(void); // Returns the keyword which is the current token. Should be called only when tokenType is KEYWORD
	char symbol(void); // Returns the character which is the current token. Should be called only when tokenType is SYMBOL
	string identifier(void); // Returns the identifier which is the current token. Should be called only when tokenType is IDENTIFIER
	int intVal(void); // Returns the integer value of the current token. Should be called only when tokenType is INT_CONST
	string stringVal(void); // Returns the string value of the current token, without the double quotes. Should be called only when tokenType is STRING_CONST
	string getCurrentToken(void); // Returns the currentToken
	void updateCurrentToken(void); // Drops off the first character of currentToken_
	
private:
	ifstream fin_;
	string currentToken_;
	
};

#endif