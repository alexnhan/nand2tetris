#include "JackTokenizer.h"
#include "JackTokenizer.cpp"
#include "CompilationEngine.h"
#include "CompilationEngine.cpp"
#include "SymbolTable.h"
#include "SymbolTable.cpp"
#include "VMWriter.h"
#include "VMWriter.cpp"
#include <iostream>
#include <string>
using namespace std;

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
				string outputFile = filename.substr(0,dotJack) + ".vm"; // output a vm file for every jack file.
				VMWriter VM(outputFile);
				SymbolTable ST;
				CompilationEngine CE(&JT,&VM,&ST);
			}
		}
	}
}