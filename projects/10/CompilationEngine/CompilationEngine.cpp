#include "CompilationEngine.h"
#include "JackTokenizer.h"
#include "JackTokenizer.cpp"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

CompilationEngine::CompilationEngine(JackTokenizer * JT, ofstream & fout)
{
	JT_ = JT;
	fout_ = fout;
}