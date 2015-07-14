#include "JackTokenizer.h"
#include "JackTokenizer.cpp"
#include "CompilationEngine.h"
#include "CompilationEngine.cpp"
#include <iostream>
#include <string>
#include <fstream>
using namespace std;

string keyWord2String(keyword Key);

int main(int argc, char *argv[])
{
	if(argc == 1)
		cerr << "Error: No files or directories provided.\n";
	else
	{
		for(int i=1;i<argc;i++)
		{
			string filename = argv[i];
			int dotJack = filename.find(".jack",0);
			if(dotJack == -1) // directory provided
			{
				cout << "Directory support currently not available. Just provide all .jack files separated by a space.\n";
				return(1);
			}
			else
			{
				JackTokenizer JT(filename);
				string outputFile = filename.substr(0,dotJack) + ".xml";
				ofstream fout(outputFile.c_str());
				fout << "<tokens>" << endl;
				while(JT.hasMoreTokens())
				{
					JT.advance();
					while(JT.getCurrentToken() != "")
					{
						switch(JT.tokenType())
						{
							case KEYWORD:
								fout << "<keyword> " << keyWord2String(JT.keyWord()) << " </keyword>" << endl;
								break;
							case SYMBOL:
								if(JT.symbol() == '<')
								{
									fout << "<symbol> " << "&lt;" << " </symbol>" << endl;
									JT.updateCurrentToken();
									break;
								}
								else if(JT.symbol() == '>')
								{
									fout << "<symbol> " << "&gt;" << " </symbol>" << endl;
									JT.updateCurrentToken();
									break;
								}
								else if(JT.symbol() == '&')
								{
									fout << "<symbol> " << "&amp;" << " </symbol>" << endl;
									JT.updateCurrentToken();
									break;
								}
								else
								{
									fout << "<symbol> " << JT.symbol() << " </symbol>" << endl;
									JT.updateCurrentToken();
									break;
								}
							case INT_CONST:
								fout << "<integerConstant> " << JT.intVal() << " </integerConstant>" << endl;
								break;
							case STRING_CONST:
								fout << "<stringConstant> " << JT.stringVal() << " </stringConstant>" << endl;
								break;
							case IDENTIFIER:
								fout << "<identifier> " << JT.identifier() << " </identifier>" << endl;
								break;
							default:
								cout << "Error: Current token not empty.\n";
								break;
						}
					}
				}
				fout << "</tokens>" << endl;
				fout.close();
			}
		}
	}
}

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