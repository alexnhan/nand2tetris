#include "CompilationEngine.h"
#include "JackTokenizer.h"
#include "SymbolTable.h"
#include "VMWriter.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
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
	
	symboltable::Kind keyWord2Kind(keyword Key)
	{
		switch(Key)
		{
			case STATIC:
				return symboltable::STATIC;
			case FIELD:
				return symboltable::FIELD;
			case VAR:
				return symboltable::VAR;
		}
	}
	
	vm::Segment kind2Segment(symboltable::Kind kind)
	{
		switch(kind)
		{
			case symboltable::STATIC:
				return vm::STATIC;
			case symboltable::FIELD:
				return vm::THIS;
			case symboltable::ARG:
				return vm::ARG;
			case symboltable::VAR:
				return vm::LOCAL;
		}
	}
}

CompilationEngine::CompilationEngine(JackTokenizer * JT, VMWriter * VM, SymbolTable * ST)
{
	JT_ = JT;
	VM_ = VM;
	ST_ = ST;
	numArgs_=0;
	labelNum_=0;
	compileClass();
}

CompilationEngine::~CompilationEngine()
{
	VM_->close();
	cout << "Compilation Complete!" << endl;
}

void CompilationEngine::compileClass(void)
{
	JT_->advance();
	assert(JT_->keyWord() == CLASS);
	JT_->advance();
	assert(JT_->tokenType() == IDENTIFIER);
	className_ = JT_->identifier();
	JT_->advance();
	assert(JT_->symbol() == '{');
	JT_->advance();
	while((JT_->tokenType() == KEYWORD) && (JT_->keyWord() == STATIC || JT_->keyWord() == FIELD || JT_->keyWord() == CONSTRUCTOR || JT_->keyWord() == FUNCTION || JT_->keyWord() == METHOD || JT_->keyWord() == VOID))
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
				compileSubroutine();
				break;
			default:
				break;
		}
	}
	assert(JT_->symbol() == '}');
}

void CompilationEngine::compileClassVarDec(void)
{
	string name, type;
	symboltable::Kind kind;
	// kind is either static or field
	kind = CEhelper::keyWord2Kind(JT_->keyWord());
	JT_->advance();

	if(JT_->tokenType() == KEYWORD) 
	{
		type = CEhelper::keyWord2String(JT_->keyWord());
		JT_->advance();
	}
	else if(JT_->tokenType() == IDENTIFIER) // a class name
	{
		type = JT_->identifier();
		JT_->advance();
	}
	while(JT_->getCurrentToken() != ";")
	{
		switch(JT_->tokenType())
		{
			case IDENTIFIER: // var name
				name = JT_->identifier();
				ST_->define(name, type, kind); // add it to symbol table
				break;
			case SYMBOL:
				JT_->updateCurrentToken(); // just eat the symbols
				break;
		}
		if(JT_->getCurrentToken() == "")
			JT_->advance();
	}
	assert(JT_->symbol() == ';');
	JT_->advance();
}

void CompilationEngine::compileSubroutine(void)
{
	ST_->startSubroutine();
	subroutineType_ = JT_->keyWord();
	JT_->advance();
	if(subroutineType_ == METHOD)
		ST_->define("this",className_,symboltable::ARG); // first argument of a method is always "this"
	string returnType;
	if(JT_->tokenType() == KEYWORD)
	{
		returnType = CEhelper::keyWord2String(JT_->keyWord());
		JT_->advance();
	}
	else if(JT_->tokenType() == IDENTIFIER)
	{
		returnType = JT_->identifier();
		JT_->advance();
	}
	if(JT_->tokenType() == IDENTIFIER)
	{
		currentFunctionName_ = className_ + "." + JT_->identifier();
		if(JT_->getCurrentToken() == "")
			JT_->advance();
		else if(JT_->symbol() == '(')
		{
			JT_->updateCurrentToken();
			if(JT_->getCurrentToken()=="")
				JT_->advance();
			compileParameterList();
		}
		if(JT_->getCurrentToken() == ")")
		{
			JT_->updateCurrentToken();
		}
		if(JT_->getCurrentToken() == "")
			JT_->advance();
		if(JT_->getCurrentToken() == "{")
			compileSubroutineBody();
	}
	JT_->advance();
}

void CompilationEngine::compileParameterList(void)
{
	string name, type;
	symboltable::Kind kind = symboltable::ARG;
	int SM = 0;
	while(JT_->getCurrentToken() != ")")
	{
		switch(JT_->tokenType())
		{
			case KEYWORD:
				if(SM == 0) 
				{
					type = CEhelper::keyWord2String(JT_->keyWord());
					JT_->advance();
					SM=1;
				}
				break;
			case IDENTIFIER:
				if(SM==1)
				{
					name = JT_->identifier();
					ST_->define(name, type, kind);
					SM=0;
				}
				else if(SM==0)
				{
					type = JT_->identifier(); // user-defined type
					JT_->advance();
					SM=1;
				}
				break;
			case SYMBOL:
				JT_->updateCurrentToken();
				SM=0;
				break;
		}
		if(JT_->getCurrentToken() == "")
			JT_->advance();
	}
}

void CompilationEngine::compileSubroutineBody(void)
{
	JT_->updateCurrentToken();
	if(JT_->getCurrentToken() == "")
		JT_->advance();
	while(JT_->keyWord() == VAR)
		compileVarDec();
	int nLocals = ST_->varCount(symboltable::VAR);
	VM_->writeFunction(currentFunctionName_, nLocals);
	if(subroutineType_ == CONSTRUCTOR)
	{
		VM_->writePush(vm::CONST,ST_->varCount(symboltable::FIELD));
		VM_->writeCall("Memory.alloc", 1);
		VM_->writePop(vm::POINTER,0);
	}
	else if(subroutineType_ == METHOD) // sets base of this segment properly, so that this one function is available to all instances of the class
	{
		VM_->writePush(vm::ARG, 0); // push "this" of calling object
		VM_->writePop(vm::POINTER, 0); // set "this" of method to the calling object
	}
	if(JT_->keyWord() == LET || JT_->keyWord() == IF || JT_->keyWord() == ELSE || JT_->keyWord() == WHILE || JT_->keyWord() == DO || JT_->keyWord() == RETURN)
		compileStatements();
	assert(JT_->symbol() == '}');
}

void CompilationEngine::compileVarDec(void)
{
	string name, type;
	symboltable::Kind kind;
	// kind is var
	kind = CEhelper::keyWord2Kind(JT_->keyWord());
	JT_->advance();
	int SM=0;
	while(JT_->getCurrentToken() != ";")
	{
		switch(JT_->tokenType())
		{
			case KEYWORD:
				if(SM==0)
				{
					type = CEhelper::keyWord2String(JT_->keyWord());
					JT_->advance();
					SM=1;
					break;
				}
			case IDENTIFIER:
				if(SM==1)
				{
					name = JT_->identifier();
					ST_->define(name, type, kind);
					break;
				}
				else if(SM==0)
				{
					type = JT_->identifier();
					JT_->advance();
					SM=1;
					break;
				}
			case SYMBOL:
				JT_->updateCurrentToken();
				break;
		}
		if(JT_->getCurrentToken() == "")
			JT_->advance();
	}
	if(JT_->getCurrentToken() == ";")
	{
		JT_->advance();
	}
}

void CompilationEngine::compileStatements(void)
{
	while((JT_->tokenType() == KEYWORD) && (JT_->keyWord() == LET || JT_->keyWord() == IF || JT_->keyWord() == ELSE || JT_->keyWord() == WHILE || JT_->keyWord() == DO || JT_->keyWord() == RETURN))
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
	}
}

void CompilationEngine::compileExpression(void)
{
	compileTerm();
	while(JT_->getCurrentToken().substr(0,1) != ";" && JT_->getCurrentToken().substr(0,1) != "}" && JT_->getCurrentToken().substr(0,1) != "]" && JT_->getCurrentToken().substr(0,1) != ")" && JT_->getCurrentToken().substr(0,1) != ",")
	{
		switch(JT_->tokenType())
		{
			char op;
			case SYMBOL:
				op = JT_->symbol();
				JT_->updateCurrentToken();
				if(JT_->getCurrentToken() == "")
					JT_->advance();
				compileTerm();
				switch(op)
				{
					case '+':
						VM_->writeArithmetic(vm::ADD);
						break;
					case '-':
						VM_->writeArithmetic(vm::SUB);
						break;
					case '*':
						VM_->writeCall("Math.multiply",2);
						break;
					case '/':
						VM_->writeCall("Math.divide",2);
						break;
					case '&':
						VM_->writeArithmetic(vm::AND);
						break;
					case '|':
						VM_->writeArithmetic(vm::OR);
						break;
					case '<':
						VM_->writeArithmetic(vm::LT);
						break;
					case '>':
						VM_->writeArithmetic(vm::GT);
						break;
					case '=':
						VM_->writeArithmetic(vm::EQ);
						break;
					case '~':
						VM_->writeArithmetic(vm::NOT);
						break;
				}
				break;
			default:
				compileTerm();
				break;
		}
		if(JT_->getCurrentToken() == "")
			JT_->advance();
	}
}

void CompilationEngine::compileTerm(void)
{
	string id, str_const;
	switch(JT_->tokenType())
	{
		case IDENTIFIER:
			id = JT_->identifier();
			if(JT_->getCurrentToken() != "")
			{
				if(JT_->symbol() == '(') // subroutine call
				{
					JT_->updateCurrentToken();
					if(JT_->getCurrentToken() == "")
						JT_->advance();
					VM_->writePush(vm::POINTER,0); // pushing this
					compileExpressionList(); // push arguments on stack
					if(JT_->symbol() == ')')
					{
						JT_->updateCurrentToken();
						if(JT_->getCurrentToken() == "")
							JT_->advance();
						if(numArgs_ > 0)
							VM_->writeCall(className_+"."+id, numArgs_+1); // numArgs aids in determining how many arguments there are, add another arg for "this"
						else // The only argument is "this"
							VM_->writeCall(className_+"."+id, 1);
						numArgs_=0; // reset for next call
					}
				}
				else if(JT_->symbol() == '.')
				{
					JT_->updateCurrentToken();
					string subroutineName = JT_->identifier();
					if(ST_->kindOf(id)!=symboltable::NONE) // if its an object stored in symboltable then we will call its own methods
					{
						VM_->writePush(CEhelper::kind2Segment(ST_->kindOf(id)),ST_->indexOf(id)); // pushing object's own reference onto stack
						if(JT_->symbol() == '(')
						{
							JT_->updateCurrentToken();
						}
						if(JT_->getCurrentToken() == "")
							JT_->advance();
						compileExpressionList();
						if(JT_->symbol() == ')')
						{
							JT_->updateCurrentToken();
							if(JT_->getCurrentToken() == "")
								JT_->advance();
						}
						VM_->writeCall(ST_->typeOf(id)+"."+subroutineName,numArgs_+1); // call predefined class's method with its own reference pushed onto stack to operate on its own fields
					}
					else // calling a built in class method, and Name already refers to its type
					{
						if(JT_->symbol() == '(')
						{
							JT_->updateCurrentToken();
						}
						if(JT_->getCurrentToken() == "")
							JT_->advance();
						compileExpressionList(); // push arguments onto stack
						if(JT_->symbol() == ')')
						{
							JT_->updateCurrentToken();
							if(JT_->getCurrentToken() == "")
								JT_->advance();
						}
						VM_->writeCall(id+"."+subroutineName,numArgs_); // call built in method
					}
					numArgs_=0;
				}
				else if(JT_->symbol() == '[')
				{
					JT_->updateCurrentToken();
					if(JT_->getCurrentToken() == "")
						JT_->advance();
					compileExpression();
					VM_->writePush(CEhelper::kind2Segment(ST_->kindOf(id)),ST_->indexOf(id));
					VM_->writeArithmetic(vm::ADD);
					if(JT_->symbol() == ']')
					{
						JT_->updateCurrentToken();
					}
					VM_->writePop(vm::POINTER,1); // pop address pointer into pointer 1
					VM_->writePush(vm::THAT,0); // push the value of the address pointer back onto stack
				}
				else
				{
					VM_->writePush(CEhelper::kind2Segment(ST_->kindOf(id)), ST_->indexOf(id));
				}
			}
			else
			{
				VM_->writePush(CEhelper::kind2Segment(ST_->kindOf(id)), ST_->indexOf(id));
			}
			break;
		case INT_CONST:
			VM_->writePush(vm::CONST, JT_->intVal());
			break;
		case STRING_CONST:
			str_const = JT_->stringVal();
			VM_->writePush(vm::CONST,str_const.length());
			VM_->writeCall("String.new",1);
			for(int i=0;i<str_const.length();i++)
			{
				VM_->writePush(vm::CONST,(int)str_const[i]);
				VM_->writeCall("String.appendChar",2);
			}
			break;
		case KEYWORD: // true, false, null, this will automatically update current token when calling keyWord()
			switch(JT_->keyWord())
			{
				case TRUE:
					VM_->writePush(vm::CONST,0);
					VM_->writeArithmetic(vm::NOT);
					break;
				case FALSE:
				case null:
					VM_->writePush(vm::CONST,0);
					break;
				case THIS:
					VM_->writePush(vm::POINTER,0); // this refers to pointer 0
					break;
			}
			break;
		case SYMBOL:
			if(JT_->symbol() == '(')
			{
				JT_->updateCurrentToken();
				if(JT_->getCurrentToken() == "")
					JT_->advance();
				compileExpression();
				assert(JT_->symbol() == ')');
				JT_->updateCurrentToken();
			}
			else //unaryOp term
			{
				char uOp = JT_->symbol();
				JT_->updateCurrentToken();
				if(JT_->getCurrentToken() == "")
					JT_->advance();
				compileTerm();
				switch(uOp)
				{
					case '-':
						VM_->writeArithmetic(vm::NEG);
						break;
					case '~':
						VM_->writeArithmetic(vm::NOT);
						break;
					default:
						cerr << "Not a unary op" << endl;
						break;
				}
			}
			break;
	}
	if(JT_->getCurrentToken() == "")
		JT_->advance();
}

void CompilationEngine::compileExpressionList(void)
{
	while(JT_->getCurrentToken().substr(0,1) != ")")
	{
		compileExpression();
		numArgs_++;
		if(JT_->getCurrentToken().substr(0,1)==",")
		{
			JT_->updateCurrentToken();
			if(JT_->getCurrentToken() == "")
				JT_->advance();
		}
	}
}

void CompilationEngine::compileDo()
{
	JT_->advance();
	string Name = JT_->identifier(); // either subroutineName or className or varName
	if(JT_->symbol() == '(') // Calling own class method, push this as first argument
	{
		JT_->updateCurrentToken();
		if(JT_->getCurrentToken() == "")
			JT_->advance();
		VM_->writePush(vm::POINTER,0); // pushing this
		compileExpressionList();
		if(JT_->symbol() == ')')
		{
			JT_->updateCurrentToken();
			if(JT_->getCurrentToken() == "")
				JT_->advance();
		}
		if(numArgs_ > 0)
			VM_->writeCall(className_+"."+Name,numArgs_+1);
		else // the only argument is "this"
			VM_->writeCall(className_+"."+Name,1);
		numArgs_=0;
	}
	else if(JT_->symbol() == '.') // Jack method, the first pushed argument must be a reference to the object on which the method is supposed to operate
	{
		JT_->updateCurrentToken();
		string subroutineName = JT_->identifier();
		if(ST_->kindOf(Name)!=symboltable::NONE)
		{
			VM_->writePush(CEhelper::kind2Segment(ST_->kindOf(Name)),ST_->indexOf(Name));
			if(JT_->symbol() == '(')
			{
				JT_->updateCurrentToken();
			}
			if(JT_->getCurrentToken() == "")
				JT_->advance();
			compileExpressionList();
			if(JT_->symbol() == ')')
			{
				JT_->updateCurrentToken();
				if(JT_->getCurrentToken() == "")
					JT_->advance();
			}
			VM_->writeCall(ST_->typeOf(Name)+"."+subroutineName,numArgs_+1);
		}
		else // calling a built in class method, and Name already refers to its type
		{
			if(JT_->symbol() == '(')
			{
				JT_->updateCurrentToken();
			}
			if(JT_->getCurrentToken() == "")
				JT_->advance();
			compileExpressionList();
			if(JT_->symbol() == ')')
			{
				JT_->updateCurrentToken();
				if(JT_->getCurrentToken() == "")
					JT_->advance();
			}
			VM_->writeCall(Name+"."+subroutineName,numArgs_);
		}
		numArgs_=0;
	}
	assert(JT_->symbol() == ';');
	JT_->advance();
	VM_->writePop(vm::TEMP,0);
}

void CompilationEngine::compileLet(void)
{
	JT_->advance();
	string LHSvar = JT_->identifier();
	int isArray = 0;
	if(JT_->getCurrentToken() == "")
		JT_->advance();
	if(JT_->symbol() == '[')
	{
		JT_->updateCurrentToken();
		if(JT_->getCurrentToken() == "")
			JT_->advance();
		compileExpression();
		VM_->writePush(CEhelper::kind2Segment(ST_->kindOf(LHSvar)),ST_->indexOf(LHSvar));
		VM_->writeArithmetic(vm::ADD); // keep address at top of stack until it is needed because if it is popped into pointer 1, it may be overwritten and lost
		if(JT_->symbol() == ']')
		{
			JT_->updateCurrentToken();
			if(JT_->getCurrentToken() == "")
				JT_->advance();
		}
		isArray=1;
	}
	if(JT_->symbol() == '=')
	{
		JT_->updateCurrentToken();
		if(JT_->getCurrentToken() == "")
			JT_->advance();
		compileExpression();
		if(isArray == 1)
			VM_->writePop(vm::TEMP,0); // store result in temp 0
	}
	assert(JT_->symbol() == ';');
	if(isArray==0)
	{
		VM_->writePop(CEhelper::kind2Segment(ST_->kindOf(LHSvar)), ST_->indexOf(LHSvar));
		JT_->advance();
	}
	else
	{
		VM_->writePop(vm::POINTER,1); // pop address pointer into pointer 1
		VM_->writePush(vm::TEMP,0); // push result back onto stack
		VM_->writePop(vm::THAT, 0); // Store right hand side evaluation in THAT 0.
		JT_->advance();
	}
}

void CompilationEngine::compileWhile(void)
{
	stringstream labelTrue, labelFalse;
	labelTrue << "WHILETRUE" << labelNum_;
	labelFalse << "WHILEFALSE" << labelNum_;
	labelNum_++;
	JT_->updateCurrentToken();
	if(JT_->getCurrentToken().substr(0,1) != "(") 
		JT_->advance();
	assert(JT_->symbol() == '(');
	JT_->updateCurrentToken();
	if(JT_->getCurrentToken() == "")
		JT_->advance();
	VM_->writeLabel(labelTrue.str()); // while true label to execute statements again
	compileExpression();
	VM_->writeArithmetic(vm::NOT);
	VM_->writeIf(labelFalse.str()); // If ~(condition) is true, skip the statements and go to the labelFalse, else run statements
	assert(JT_->symbol() == ')');
	JT_->updateCurrentToken();
	if(JT_->getCurrentToken() == "")
		JT_->advance();
	assert(JT_->symbol() == '{');
	JT_->updateCurrentToken();
	if(JT_->getCurrentToken() == "")
		JT_->advance();
	compileStatements();
	VM_->writeGoto(labelTrue.str()); // Go back to labelTrue and check condition
	VM_->writeLabel(labelFalse.str()); // Breaks out of while loop because ~(condition) is true
	assert(JT_->symbol() == '}');
	JT_->updateCurrentToken();
	if(JT_->getCurrentToken() == "")
		JT_->advance();
}

void CompilationEngine::compileReturn(void)
{
	if(JT_->getCurrentToken() == "return")
	{
		JT_->advance();
		compileExpression();
		assert(JT_->symbol() == ';');
		VM_->writeReturn();
		JT_->advance();
	}
	else if(JT_->getCurrentToken() == "return;")
	{
		VM_->writePush(vm::CONST,0);
		VM_->writeReturn();
		JT_->advance();
	}
}

void CompilationEngine::compileIf(void)
{
	stringstream labelElse, labelContinue;
	labelElse << "IFFALSE" << labelNum_;
	labelContinue << "CONTINUE" << labelNum_;
	labelNum_++;
	JT_->updateCurrentToken();
	JT_->updateCurrentToken(); // done twice since "if" is 2 letters
	if(JT_->getCurrentToken() == "")
		JT_->advance();
	assert(JT_->symbol() == '(');
	JT_->updateCurrentToken();
	if(JT_->getCurrentToken() == "")
		JT_->advance();
	compileExpression();
	VM_->writeArithmetic(vm::NOT);
	VM_->writeIf(labelElse.str());
	assert(JT_->symbol() == ')');
	JT_->updateCurrentToken();
	if(JT_->getCurrentToken() == "")
		JT_->advance();
	assert(JT_->symbol() == '{');
	JT_->updateCurrentToken();
	if(JT_->getCurrentToken() == "")
		JT_->advance();
	compileStatements();
	VM_->writeGoto(labelContinue.str());
	assert(JT_->symbol() == '}');
	JT_->updateCurrentToken();
	if(JT_->getCurrentToken() == "")
		JT_->advance();
	VM_->writeLabel(labelElse.str());
	if(JT_->tokenType() == KEYWORD && JT_->keyWord() == ELSE)
	{
		JT_->updateCurrentToken();
		JT_->updateCurrentToken();
		JT_->updateCurrentToken();
		JT_->updateCurrentToken(); // done 4 times because "else" is 4 letters
		if(JT_->getCurrentToken() == "")
			JT_->advance();
		assert(JT_->symbol() == '{');
		JT_->updateCurrentToken();
		if(JT_->getCurrentToken() == "")
			JT_->advance();
		compileStatements();
		assert(JT_->symbol() == '}');
		JT_->updateCurrentToken();
		if(JT_->getCurrentToken() == "")
			JT_->advance();
	}
	VM_->writeLabel(labelContinue.str());
}