#pragma once

namespace tinyxml2
{	class XMLAttribute;
};

class sAttributeNode
{
public:
	sAttributeNode() {}
	sAttributeNode(const char *name, const char *value, const tinyxml2::XMLAttribute *_ptr)
		: ptr(_ptr)
	{
		this->name = name;
		this->value = value;
	}
	std::string name;
	std::string value;
	const tinyxml2::XMLAttribute *ptr;
};

