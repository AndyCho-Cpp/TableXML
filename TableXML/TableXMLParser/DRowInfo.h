#pragma once
#include <string>
#include <map>
#include <vector>
#include "tinyxml2_helper_attribute_node.hpp"

namespace tinyxml2
{
	class XMLDocument;
	class XMLNode;
	class XMLAttribute;
	class XMLElement;
	//enum XMLError;
};
class DColumnInfo;
class AttributeNodeEx : public sAttributeNode
{
public:
	AttributeNodeEx() {}
	AttributeNodeEx(sAttributeNode &parent)
	{
		sAttributeNode *pThis = this;
		*pThis = parent;
	}
	DColumnInfo *colInfo; // column info ptr
};

class DRowInfo
{
public:
	DRowInfo();
	~DRowInfo();
	bool Parse(const std::map<std::string, DColumnInfo*> *colHash, const tinyxml2::XMLNode* node, std::string *errMsg);
	void SetKey(std::string key);

	const tinyxml2::XMLNode *element;

	std::map<std::string/*Column Name*/, AttributeNodeEx*> subItems;

	std::string stringKey;

};

