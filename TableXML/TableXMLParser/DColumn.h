#pragma once
#include <string>
enum class ValueType : char
{
	eBool, eChar, eInt, eDouble, eString
};

class DColumn
{
public:
	DColumn();
	~DColumn();
	
	int colNo;
	std::string name;
	ValueType type;
	std::string defaultValue;
	
};

