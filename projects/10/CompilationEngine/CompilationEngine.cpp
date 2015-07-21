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

CompilationEngine::CompilationEngine(JackTokenizer * JT_, string outputFile)
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
	JT_->advance();
	assert(JT_->keyWord() == CLASS);
	fout_ << "<keyword> " << "class" << " </keyword>" << endl;
	JT_->advance();
	assert(JT_->tokenType() == IDENTIFIER);
	fout_ << "<identifier> " << JT_->identifier() << " </identifier>" << endl;
	JT_->advance();
	assert(JT_->symbol() == '{');
	fout_ << "<symbol> { </symbol>" << endl;
	JT_->advance();
	while(JT_->keyWord() == STATIC || JT_->keyWord() == FIELD || JT_->keyWord() == CONSTRUCTOR || JT_->keyWord() == FUNCTION || JT_->keyWord() == METHOD || JT_->keyWord() == VOID)
	{
		switch(JT_->keyWord())
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
	JT_->advance();
	assert(JT_->symbol() == '}');
	fout_ << "<symbol> } </symbol>" << endl;
	fout_ << "</class>" << endl;
}

void CompilationEngine::compileClassVarDec(void)
{
	fout_ << "<classVarDec>" << endl;
	fout_ << "<keyword> " << CEhelper::keyWord2String(JT_->keyWord()) << " </keyword>" << endl;
	JT_->advance();
	if(JT_->tokenType() == KEYWORD)
	{
		fout_ << "<keyword> " << CEhelper::keyWord2String(JT_->keyWord()) << " </keyword>" << endl; 
		JT_->advance();
	}
	else if(JT_->tokenType() == IDENTIFIER)
	{
		fout_ << "<identifier> " << JT_->identifier() << " </identifier>" << endl;
		JT_->advance();
	}
	while(JT_->getCurrentToken() != ";")
	{
		switch(JT_->tokenType())
		{
			case IDENTIFIER:
				fout_ << "<identifier> " << JT_->identifier() << " </identifier>" << endl;
				break;
			case SYMBOL:
				fout_ << "<symbol> " << JT_->symbol() << " </symbol>" << endl;
				JT_->updateCurrentToken();
				break;
		}
		if(JT_->getCurrentToken() == "")
			JT_->advance();
	}
	assert(JT_->symbol() == ';');
	fout_ << "<symbol> ; </symbol>" << endl;
	fout_ << "</classVarDec>" << endl;
}

void CompilationEngine::compileSubroutine(void)
{
	fout_ << "<subroutineDec>" << endl;
	fout_ << "<keyword> " << CEhelper::keyWord2String(JT_->keyWord()) << " </keyword>" << endl;
	JT_->advance();
	if(JT_->tokenType() == KEYWORD)
	{
		fout_ << "<keyword> " << CEhelper::keyWord2String(JT_->keyWord()) << " </keyword>" << endl;
		JT_->advance();
	}
	else if(JT_->tokenType() == IDENTIFIER)
	{
		fout_ << "<identifier> " << JT_->identifier() << " </identifier>" << endl;
		JT_->advance();
	}
	if(JT_->tokenType() == IDENTIFIER)
	{
		fout_ << "<identifier> " << JT_->identifier() << " </identifier>" << endl;
		if(JT_->getCurrentToken() == "")
			JT_->advance();
		else if(JT_->symbol() == '(')
		{
			fout_ << "<symbol> " << JT_->symbol() << " </symbol>" << endl;
			JT_->updateCurrentToken();
			if(JT_->getCurrentToken() != ")") // call it if its not an empty parameter list
				compileParameterList();
		}
		if(JT_->getCurrentToken() == ")")
		{
			fout_ << "<symbol> " << JT_->symbol() << " </symbol>" << endl;
			JT_->updateCurrentToken();
		}
		if(JT_->getCurrentToken() == "")
			JT_->advance();
		if(JT_->getCurrentToken() == "{")
			subroutineBody();
	}
	fout_ << "</subroutineDec>" << endl;
}

void CompilationEngine::compileParameterList(void)
{
	fout_ << "<parameterList>" << endl;
	while(JT_->getCurrentToken() != ")")
	{
		switch(JT_->tokenType())
		{
			case KEYWORD:
				fout_ << "<keyword> " << CEhelper::keyWord2String(JT_->keyWord()) << " </keyword>" << endl;
				JT_->advance();
				break;
			case IDENTIFIER:
				fout_ << "<identifier> " << JT_->identifier() << " </identifier>" << endl;
				break;
			case SYMBOL:
				fout_ << "<symbol> " << JT_->symbol() << " </symbol>" << endl;
				JT_->updateCurrentToken();
				break;
		}
		if(JT_->getCurrentToken() == "")
			JT_->advance();
	}
	fout_ << "</parameterList>" << endl;
}

void CompilationEngine::compileSubroutineBody(void)
{
	fout_ << "<subroutineBody>" << endl;
	fout_ << "<symbol> " << "{" << " </symbol>" << endl;
	JT_->updateCurrentToken();
	if(JT_->getCurrentToken() == "")
		JT_->advance();
	if(JT_->keyWord() == VAR)
		compileVarDec();
	if(JT_->keyWord() == LET || JT_->keyWord() == IF || JT_->keyWord() == ELSE || JT_->keyWord() == WHILE || JT_->keyWord() == DO || JT_->keyWord() == RETURN)
		compileStatements();
	if(JT_->symbol() == '}')
		fout_ << "<symbol> " << JT_->symbol() << " </symbol>" << endl;
	JT_->advance();
	fout_ << "</subroutineBody>" << endl;
}

void CompilationEngine::compileVarDec(void)
{
	fout_ << "<varDec>" << endl;
	fout_ << "<keyword> " << CEhelper::keyWord2String(JT_->keyWord()) << " </keyword>" << endl;
	JT_->advance();
	while(JT_->getCurrentToken() != ";")
	{
		switch(JT_->tokenType())
		{
			case KEYWORD:
				fout_ << "<keyword> " << CEhelper::keyWord2String(JT_->keyWord()) << " </keyword>" << endl;
				JT_->advance();
				break;
			case IDENTIFIER:
				fout_ << "<identifier> " << JT_->identifier() << " </identifier>" << endl;
				break;
			case SYMBOL:
				fout_ << "<symbol> " << JT_->symbol() << " </symbol>" << endl;
				JT_->updateCurrentToken();
				break;
		}
		if(JT_->getCurrentToken() == "")
			JT_->advance();
	}
	if(JT_->getCurrentToken() == ";")
	{
		fout_ << "<symbol> " << JT_->symbol() << " </symbol>" << endl;
		JT_->advance();
	}
	fout_ << "</varDec>" << endl;
}

void CompilationEngine::compileStatements(void)
{
	fout_ << "<statements>" << endl;
	while(JT_->keyWord() == LET || JT_->keyWord() == IF || JT_->keyWord() == ELSE || JT_->keyWord() == WHILE || JT_->keyWord() == DO || JT_->keyWord() == RETURN)
	{
		switch(JT_->keyWord())
		{
			case LET:
				compileLet();
				break;
			case DO:
				compileDo();
				break;
			case WHILE:
				compileWhile();
				break;
			case RETURN:
				compileReturn();
				break;
			case IF:
				compileIf();
				break;
		}
		if(JT_->getCurrentToken() == "")
			JT_->advance();
	}
	fout_ << "</statements>" << endl;
}

void CompilationEngine::compileExpression(void)
{
	fout_ << "<expression>" << endl;
	compileTerm();
	while(JT_->getCurrentToken() != ";" && JT_->getCurrentToken() != "}" && JT_->getCurrentToken() != "]" && JT_->getCurrentToken() != ")")
	{
		switch(JT_->tokenType())
		{
			case SYMBOL:
				fout_ << "<symbol> " << JT_->symbol() << " </symbol>" << endl;
				JT_->updateCurrentToken();
				if(JT_->getCurrentToken() == "")
					JT_->advance();
				break;
			default:
				compileTerm();
				break;
		}
		if(JT_->getCurrentToken() == "")
			JT_->advance();
	}
	fout_ << "</expression>" << endl;
}

void CompilationEngine::compileTerm(void)
{
	fout_ << "<term>" << endl;
	while(JT_->getCurrentToken() != ";")
	{
		switch(JT_->tokenType())
		{
			case IDENTIFIER:
				fout_ << "<identifier> " << JT_->identifier() << " </identifier>" << endl;
				if(JT_->symbol() == '(') // subroutine call
				{
					fout_ << "<symbol> " << JT_->symbol() << " </symbol>" << endl;
					JT_->updateCurrentToken();
					if(JT_->getCurrentToken() == "")
						JT_->advance();
					compileExpressionList();
					if(JT_->symbol() == ")")
					{
						fout_ << "<symbol> " << JT_->symbol() << " </symbol>" << endl;
						JT_->advance();
					}
				}
				else if(JT_->symbol() == ".")
				{
					fout_ << "<symbol> " << JT_->symbol() << " </symbol>" << endl;
					JT_->updateCurrentToken();
					fout_ << "<identifier> " << JT_->identifier() << " </identifier>" << endl;
					if(JT_->symbol() == '(')
					{
						fout_ << "<symbol> " << JT_->symbol() << " </symbol>" << endl;
						JT_->updateCurrentToken();
					}
					if(JT_->getCurrentToken() == "")
						JT_->advance();
					compileExpressionList();
					if(JT_->symbol() == ')')
					{
						fout_ << "<symbol> " << JT_->symbol() << " </symbol>" << endl;
						JT_->updateCurrentToken();
					}
				}
				else if(JT_->symbol() == "[")
				{
					fout_ << "<symbol> " << JT_->symbol() << " </symbol>" << endl;
					JT_->updateCurrentToken();
					if(JT_->getCurrentToken() == "")
						JT_->advance();
					compileExpression();
					if(JT_->symbol() == ']')
					{
						fout_ << "<symbol> " << JT_->symbol() << " </symbol>" << endl;
						JT_->updateCurrentToken();
					}
				}
				break;
			case INT_CONST:
				fout_ << "<integerConstant> " << JT_->intVal() << " </integerConstant>" << endl;
				break;
			case STRING_CONST:
				fout_ << "<stringConstant> " << JT_->stringVal() << " </stringConstant>" << endl;
				break;
			case KEYWORD:
				fout_ << "<keyword> " << JT_->keyWord() << " </keyword>" << endl;
				break;
			case SYMBOL:
				fout_ << "<symbol> " << JT_->symbol() << " </symbol>" << endl;
				JT_->updateCurrentToken();
				break;
		}
		if(JT_->getCurrentToken() == "")
			JT_->advance();
	}
	fout_ << "</term>" << endl;
}

void CompilationEngine::compileExpressionList(void)
{
	fout_ << "<expressionList>" << endl;
	while(JT_->getCurrentToken() != ")")
	{
		compileExpression();
	}
	fout_ << "</expressionList>" << endl;
}

void CompilationEngine::compileDo()
{
	fout_ << "<doStatement>" << endl;
	fout_ << "<keyword> " << CEhelper::keyWord2String(JT_->keyWord()) << " </keyword>" << endl;
	JT_->advance();
	compileExpression();
	if(JT_->symbol() == ';')
		fout_ << "<symbol> " << JT_->symbol() << " </symbol>" << endl;
	JT_->advance();
	fout_ << "</doStatement>" << endl;
}

void CompilationEngine::compileLet(void)
{
	fout_ << "<letStatement>" << endl;
	fout_ << "<keyword> " << CEhelper::keyWord2String(JT_->keyWord()) << " </keyword>" << endl;
	JT_->advance();
	fout_ << "<identifier> " << JT_->identifier() << " </identifier>" << endl;
	JT_->updateCurrentToken();
	if(JT_->symbol() == '[')
	{
		fout_ << "<symbol> " << JT_->symbol() << " </symbol>" << endl;
		JT_->updateCurrentToken();
		if(JT_->getCurrentToken() == "")
			JT_->advance();
		compileExpression();
		if(JT_->symbol() == ']')
		{
			fout_ << "<symbol>" << JT_->symbol() << " </symbol>" << endl;
			JT_->updateCurrentToken();
			if(JT_->getCurrentToken() == "")
				JT_->advance();
		}
	}
	if(JT_->symbol() == '=')
	{
		fout_ << "<symbol> " << JT_->symbol() << " </symbol>" << endl;
		JT_->updateCurrentToken();
		if(JT_->getCurrentToken() == "")
			JT_->advance();
		compileExpression();
	}
	if(JT_->symbol() == ';')
	{
		fout_ << "<symbol> " << JT_->symbol() << " </symbol>" << endl;
	}
	JT->advance();
	fout_ << "</letStatement>" << endl;
}

void CompilationEngine::compileWhile(void)
{
	fout_ << "<whileStatement>" << endl;
	fout_ << "<keyword> " << CEhelper::keyWord2String(JT_->keyWord()) << " </keyword>" << endl;
	JT_->updateCurrentToken();
	if(JT_->getCurrentToken() == "")
		JT_->advance();
	assert(JT_->symbol() == '(');
	fout_ << "<symbol> " << JT_->symbol() << " </symbol>" << endl;
	JT_->updateCurrentToken();
	if(JT_->getCurrentToken() == "")
		JT_->advance();
	compileExpression();
	assert(JT_->symbol() == ')');
	fout_ << "<symbol> " << JT_->symbol() << " </symbol>" << endl;
	JT_->updateCurrentToken();
	if(JT_->getCurrentToken() == "")
		JT_->advance();
	assert(JT_->symbol() == '{');
	fout_ << "<symbol> " << JT_->symbol() << " </symbol>" << endl;
	JT_->updateCurrentToken();
	if(JT_->getCurrentToken() == "")
		JT_->advance();
	compileStatements();
	assert(JT_->symbol() == '}');
	fout_ << "<symbol> " << JT_->symbol() << " </symbol>" << endl;
	JT_->updateCurrentToken();
	if(JT_->getCurrentToken() == "")
		JT_->advance();
}

void CompilationEngine::compileReturn(void)
{
	
	
}