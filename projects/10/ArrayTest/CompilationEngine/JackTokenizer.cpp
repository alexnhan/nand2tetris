#include "JackTokenizer.h"
#include <iostream>
#include <string>
#include <assert.h>
#include <fstream>
using namespace std;

// helper functions
namespace helper
{
	bool isInt(string currentToken)
	{
		if(currentToken.empty()) { return false; } // make sure the string is not empty
		for(int i=0;i<currentToken.length();i++)
		{
			if(currentToken[i] != '0' && currentToken[i] != '1' && currentToken[i] != '2' && currentToken[i] != '3' && currentToken[i] != '4' && currentToken[i] != '5'
				&& currentToken[i] != '6' && currentToken[i] != '7' && currentToken[i] != '8' && currentToken[i] != '9') { return false; }
		}
		// number is an int, check the range
		int temp = stoi(currentToken);
		assert(temp >= 0 && temp <= 32767);
		return true; // int is in range, return true
	}
}

// Constructor
JackTokenizer::JackTokenizer(string inputFile)
{
	fin_.open(inputFile.c_str());
	if(!fin_.is_open())
	{
		cerr << "JackTokenizer: Failed to open input file.\n";
	}
	currentToken_="";
}

// Destructor
JackTokenizer::~JackTokenizer()
{
	if(fin_.is_open())
		fin_.close();
}

// Methods
bool JackTokenizer::hasMoreTokens(void)
{
	return !fin_.eof();
}

void JackTokenizer::advance(void)
{
	if(hasMoreTokens())
		fin_ >> currentToken_;
	// Handling comments
	while(currentToken_ == "//" || currentToken_ == "/*" || currentToken_ == "/**")
	{
		if(currentToken_ == "/*" || currentToken_ == "/**")
		{
			int endcomment;
			do
			{
				getline(fin_,currentToken_);
				endcomment = currentToken_.find("*/");
			} while(endcomment == -1);
			fin_ >> currentToken_;
		}
		else
		{
			getline(fin_,currentToken_); // get the rest of the line and trash it
			fin_ >> currentToken_; // get first token of next line
		}
	}
}

tokentype JackTokenizer::tokenType(void)
{
	if(currentToken_ == "class" || currentToken_ == "constructor" || currentToken_ == "function" || currentToken_ == "method" ||
		currentToken_ == "field" || currentToken_ == "static" || currentToken_ == "var" || currentToken_ == "int" || currentToken_ == "char" ||
		currentToken_ == "boolean" || currentToken_ == "void" || currentToken_ == "true" || currentToken_ == "true;" || currentToken_ == "true)" || currentToken_ == "true);" ||
		currentToken_ == "false" || currentToken_ == "false;" || currentToken_ == "false)" || currentToken_ == "false);" || currentToken_ == "null" || currentToken_ == "null;" || currentToken_ == "null)" || currentToken_ == "null);" ||
		currentToken_ == "this" || currentToken_ == "this;" || currentToken_ == "this)" || currentToken_ == "this);" || currentToken_ == "let" || currentToken_ == "do" || currentToken_ == "if" || currentToken_ == "else" ||
		currentToken_ == "while" || currentToken_ == "return" || currentToken_ == "return;") { return KEYWORD; }
	else if(currentToken_[0] == '{' || currentToken_[0] == '}' || currentToken_[0] == '(' || currentToken_[0] == ')' || currentToken_[0] == '[' || currentToken_[0] == ']'
		 || currentToken_[0] == '.' || currentToken_[0] == ',' || currentToken_[0] == ';' || currentToken_[0] == '+' || currentToken_[0] == '-' || currentToken_[0] == '*'
		 || currentToken_[0] == '/' || currentToken_[0] == '&' || currentToken_[0] == '|' || currentToken_[0] == '<' || currentToken_[0] == '>' || currentToken_[0] == '='
		 || currentToken_[0] == '~') { return SYMBOL; }
	else if(helper::isInt(currentToken_.substr(0,1))) { return INT_CONST; }
	else if(currentToken_[0] == '"') { return STRING_CONST; }
	else if(!helper::isInt(currentToken_.substr(0,1))) { return IDENTIFIER; } // variable name does not start with a number
	else { cerr << "Token is not a token type.\n"; }
}

keyword JackTokenizer::keyWord(void)
{
	assert(tokenType() == KEYWORD);
	if(currentToken_ == "class") 
	{
		return CLASS;
	}
	else if(currentToken_ == "method") 
	{
		return METHOD;
	}
	else if(currentToken_ == "function") 
	{
		return FUNCTION;
	}
	else if(currentToken_ == "constructor")
	{
		return CONSTRUCTOR;
	}
	else if(currentToken_ == "int")
	{
		return INT;
	}
	else if(currentToken_ == "boolean") 
	{
		return BOOLEAN;
	}
	else if(currentToken_ == "char")
	{
		return CHAR;
	}
	else if(currentToken_ == "void")
	{
		return VOID;
	}
	else if(currentToken_ == "var")
	{
		return VAR;
	}
	else if(currentToken_ == "static") 
	{
		return STATIC;
	}
	else if(currentToken_ == "field")
	{
		return FIELD;
	}
	else if(currentToken_ == "let")
	{
		return LET;
	}
	else if(currentToken_ == "do") 
	{
		return DO;
	}
	else if(currentToken_ == "if")
	{
		return IF;
	}
	else if(currentToken_ == "else") 
	{
		return ELSE;
	}
	else if(currentToken_ == "while") 
	{
		return WHILE;
	}
	else if(currentToken_ == "return" || currentToken_ == "return;") 
	{
		if(currentToken_ == "return")
			currentToken_ = "";
		else
			currentToken_ = ";";
		return RETURN;
	}
	else if(currentToken_ == "true" || currentToken_ == "true;" || currentToken_ == "true)" || currentToken_ == "true);") 
	{
		if(currentToken_ == "true")
			currentToken_ = "";
		else if(currentToken_ == "true;")
			currentToken_ = ";";
		else if(currentToken_=="true)")
			currentToken_ = ")";
		else
			currentToken_=");";
		return TRUE;
	}
	else if(currentToken_ == "false" || currentToken_ == "false;" || currentToken_ == "false)" || currentToken_ == "false);") 
	{
		if(currentToken_ == "false")
			currentToken_ = "";
		else if(currentToken_ == "false;")
			currentToken_ = ";";
		else if(currentToken_=="false)")
			currentToken_ = ")";
		else
			currentToken_=");";
		return FALSE;
	}
	else if(currentToken_ == "null" || currentToken_ == "null;" || currentToken_ == "null)" || currentToken_ == "null);") 
	{
		if(currentToken_=="null")
			currentToken_="";
		else if(currentToken_ == "null;")
			currentToken_ = ";";
		else if(currentToken_=="null)")
			currentToken_=")";
		else
			currentToken_=");";
		return null;
	}
	else if(currentToken_ == "this" || currentToken_ == "this;" || currentToken_=="this)" || currentToken_=="this);") 
	{
		if(currentToken_=="this")
			currentToken_="";
		else if(currentToken_ == "this;")
			currentToken_ = ";";
		else if(currentToken_=="this)")
			currentToken_=")";
		else
			currentToken_=");";
		return THIS;
	}
}

char JackTokenizer::symbol(void)
{
	assert(tokenType() == SYMBOL);
	return currentToken_[0];
}

string JackTokenizer::identifier(void) // identifier will perform an update on currentToken_
{
	assert(tokenType() == IDENTIFIER);
	string returnString;
	int dot = currentToken_.find(".");
	int parens = currentToken_.find("(");
	int braket = currentToken_.find("[");
	int endparens = currentToken_.find(")");
	int endbraket = currentToken_.find("]");
	int comma = currentToken_.find(",");
	int semi = currentToken_.find(";");
	int eq = currentToken_.find("=");
	if(dot > -1)
	{
		returnString += currentToken_.substr(0,dot);
		currentToken_ = currentToken_.substr(dot, currentToken_.length());
	}
	else if(parens > -1)
	{
		returnString += currentToken_.substr(0,parens);
		currentToken_ = currentToken_.substr(parens, currentToken_.length());
	}
	else if(braket > -1)
	{
		returnString += currentToken_.substr(0,braket);
		currentToken_ = currentToken_.substr(braket, currentToken_.length());
	}
	else if(endparens > -1)
	{
		returnString += currentToken_.substr(0,endparens);
		currentToken_ = currentToken_.substr(endparens, currentToken_.length());
	}
	else if(endbraket > -1)
	{
		returnString += currentToken_.substr(0,endbraket);
		currentToken_ = currentToken_.substr(endbraket, currentToken_.length());
	}
	else if(comma > -1)
	{
		returnString += currentToken_.substr(0,comma);
		currentToken_ = currentToken_.substr(comma, currentToken_.length());
	}
	else if(eq > -1)
	{
		returnString += currentToken_.substr(0,eq);
		currentToken_ = currentToken_.substr(eq,currentToken_.length());
	}
	else if(semi > -1) // semi is last because a statement always ends in semicolon
	{
		returnString += currentToken_.substr(0,semi);
		currentToken_ = currentToken_.substr(semi, currentToken_.length());
	}
	else
	{
		returnString = currentToken_;
		currentToken_ = "";
	}
	return returnString;
}

int JackTokenizer::intVal(void)
{
	assert(tokenType() == INT_CONST);
	string returnInt;
	returnInt += currentToken_[0];
	currentToken_ = currentToken_.substr(1,currentToken_.length());
	while(helper::isInt(currentToken_.substr(0,1)))
	{
		returnInt += currentToken_[0];
		currentToken_ = currentToken_.substr(1,currentToken_.length());
	}	
	return stoi(returnInt);
}

string JackTokenizer::stringVal(void)
{
	assert(tokenType() == STRING_CONST);
	char c;
	fin_.get(c);
	while(c != '"')
	{
		currentToken_ += c;
		fin_.get(c);
	}
	string returnString = currentToken_.substr(1,currentToken_.length()-1);
	currentToken_ = "";
	return returnString;
}

string JackTokenizer::getCurrentToken(void)
{
	return currentToken_;
}

void JackTokenizer::updateCurrentToken(void)
{
	currentToken_ = currentToken_.substr(1,currentToken_.length());
}