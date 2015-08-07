#ifndef _SYMBOLTABLE_
#define _SYMBOLTABLE_

#include <string>
#include <map>
using namespace std;

namespace symboltable
{
	enum Kind { STATIC, FIELD, ARG, VAR, NONE };
	
	struct ST
	{
		string type;
		Kind kind;
		int index;
	};
}

class SymbolTable
{
public:	
	// Constructor
	SymbolTable(); // Creates a new empty symbol table
	
	// Methods
	void startSubroutine(void); // Starts a new subroutine scope (resets the subroutine's symbol table)
	void define(string name, string type, symboltable::Kind kind); // Defines a new identifier of a given name, type, and kind and assigns it a running index. symboltable::STATIC and FIELD have class scope, ARG and VAR have subroutine scope
	int varCount(symboltable::Kind kind); // Returns the number of variables of the given kind already defined in the current scope
	symboltable::Kind kindOf(string name); // Returns the kind of the named identifier in the current scope. If the identifier is unknown in the current scope, returns NONE
	string typeOf(string name); // Returns the type of the named identifier in the current scope
	int indexOf(string name); // Returns the index assigned to the named identifier

	
private:
	map<string, symboltable::ST> class_scope_; // hash table for class scope
	map<string, symboltable::ST> subroutine_scope_; // hash table for subroutine scope
	int static_count_; // keeps a running index of static variables for class scope
	int field_count_; // keeps a running index of field variables for class scope
	int arg_count_; // keeps a running index of argument variables for subroutine scope
	int var_count_; // keeps a running index of var variables for subroutine scope
};

#endif