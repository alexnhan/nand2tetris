#include "CompilationEngine.h"
#include "JackTokenizer.h"
#include <iostream>
#include <fstream>
#include <string>
#include <assert.h>
using namespace std;

namespace CEhelper
{
	string keyWord2String(keyword Key)
	{
		switch(Key)
		{
			case CLASS:
				return "class";
			case METHOD:
				return "method";
			case FUNCTION:
				return "function";
			case CONSTRUCTOR:
				return "constructor";
			case INT:
				return "int";
			case BOOLEAN:
				return "boolean";
			case CHAR:
				return "char";
			case VOID:
				return "void";
			case VAR:
				return "var";
			case STATIC:
				return "static";
			case FIELD:
				return "field";
			case LET:
				return "let";
			case DO:
				return "do";
			case IF:
				return "if";
			case ELSE:
				return "else";
			case WHILE:
				return "while";
			case RETURN:
				return "return";
			case TRUE:
				return "true";
			case FALSE:
				return "false";
			case null:
				return "null";
			case THIS:
				return "this";
		}
	}
}

CompilationEngine::CompilationEngine(JackTokenizer * JT, string outputFile)
{
	JT_ = JT;
	fout_.open(outputFile.c_str());
	if(!fout_.is_open())
		cerr << "Failed to open output file.\n";
	compileClass();
}

CompilationEngine::~CompilationEngine()
{
	if(fout_.is_open())
		fout_.close();
}

void CompilationEngine::compileClass(void)
{
	fout_ << "<class>" << endl; // class header
	JT->advance();
	assert(JT->keyWord() == CLASS);
	fout_ << "<keyword> " << "class" << " </keyword>" << endl;
	JT->advance();
	assert(JT->tokenType() == IDENTIFIER);
	fout_ << "<identifier> " << JT->identifier() << " </identifier>" << endl;
	JT->advance();
	assert(JT->symbol() == '{');
	fout_ << "<symbol> { </symbol>" << endl;
	JT->advance();
	while(JT->keyWord() == STATIC || JT->keyWord() == FIELD || JT->keyWord() == CONSTRUCTOR || JT->keyWord() == FUNCTION || JT->keyWord() == METHOD || JT->keyWord() == VOID)
	{
		switch(JT->keyWord())
		{
			case STATIC:
			case FIELD:
				compileClassVarDec();
				break;
			case CONSTRUCTOR:
			case FUNCTION:
			case METHOD:
			case VOID:
				compileSubroutine();
				break;
			default:
				break;
		}
	}
	JT->advance();
	assert(JT->symbol() == '}');
	fout_ << "<symbol> } </symbol>" << endl;
	fout_ << "</class>" << endl;
}

void CompilationEngine::compileClassVarDec(void)
{
	fout_ << "<classVarDec>" << endl;
	fout_ << "<keyword> " << CEhelper::keyWord2String(JT->keyWord()) << " </keyword>" << endl;
	JT->advance();
	if(JT->tokenType() == KEYWORD)
	{
		fout_ << "<keyword> " << CEhelper::keyWord2String(JT->keyWord()) << " </keyword>" << endl; 
		JT->advance();
	}
	else if(JT->tokenType() == IDENTIFIER)
	{
		fout_ << "<identifier> " << JT->identifier() << " </identifier>" << endl;
		JT->advance();
	}
	while(JT->getCurrentToken() != ";")
	{
		switch(JT->tokenType())
		{
			case IDENTIFIER:
				fout_ << "<identifier> " << JT->identifier() << " </identifier>" << endl;
				break;
			case SYMBOL:
				fout_ << "<symbol> " << JT->symbol() << " </symbol>" << endl;
				JT->updateCurrentToken();
				break;
		}
		if(JT->getCurrentToken() == "")
			JT->advance();
	}
	assert(JT->symbol() == ';');
	fout_ << "<symbol> ; </symbol>" << endl;
	fout_ << "</classVarDec>" << endl;
}