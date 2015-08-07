#include "SymbolTable.h"
#include <map>
#include <string>
using namespace std;

// helper method
bool isKey(map<string, symboltable::ST> & scope, string key)
{
	return !(scope.find(key) == scope.end());
}

SymbolTable::SymbolTable() // all private variables initialized as ""empty"
{
	class_scope_.clear();
	subroutine_scope_.clear();
	static_count_ = 0;
	field_count_ = 0;
	arg_count_ = 0;
	var_count_ = 0; 
}

void SymbolTable::startSubroutine(void) // resets the subroutine's symbol table
{
	subroutine_scope_.clear();
	arg_count_ = 0;
	var_count_ = 0; 
}

void SymbolTable::define(string name, string type, symboltable::Kind kind)
{
	if(kind == symboltable::STATIC)
	{
		class_scope_[name].type = type;
		class_scope_[name].kind = kind;
		class_scope_[name].index = static_count_;
		static_count_++;
	}
	else if(kind == symboltable::FIELD)
	{
		class_scope_[name].type = type;
		class_scope_[name].kind = kind;
		class_scope_[name].index = field_count_;
		field_count_++;
	}
	else if(kind == symboltable::ARG)
	{
		subroutine_scope_[name].type = type;
		subroutine_scope_[name].kind = kind;
		subroutine_scope_[name].index = arg_count_;
		arg_count_++;
	}
	else if(kind == symboltable::VAR)
	{
		subroutine_scope_[name].type = type;
		subroutine_scope_[name].kind = kind;
		subroutine_scope_[name].index = var_count_;
		var_count_++;
	}
}

int SymbolTable::varCount(symboltable::Kind kind)
{
	switch(kind)
	{
		case symboltable::STATIC:
			return static_count_;
		case symboltable::FIELD:
			return field_count_;
		case symboltable::ARG:
			return arg_count_;
		case symboltable::VAR:
			return var_count_;
		default:
			return -1;
	}
}

symboltable::Kind SymbolTable::kindOf(string name)
{
	if(isKey(class_scope_, name))
		return class_scope_[name].kind;
	else if(isKey(subroutine_scope_, name))
		return subroutine_scope_[name].kind;
	else
		return symboltable::NONE;
}

string SymbolTable::typeOf(string name)
{
	if(isKey(class_scope_, name))
		return class_scope_[name].type;
	else if(isKey(subroutine_scope_, name))
		return subroutine_scope_[name].type;
	else
		return "";
}

int SymbolTable::indexOf(string name)
{
	if(isKey(class_scope_, name))
		return class_scope_[name].index;
	else if(isKey(subroutine_scope_, name))
		return subroutine_scope_[name].index;
	else
		return -1;
}