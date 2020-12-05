#pragma once
#include <string>

namespace tinyxml2
{
	class XMLDocument;
	class XMLNode;
	class XMLAttribute;
	class XMLElement;
	//enum XMLError;
};

struct sXmlFileVersion
{
	sXmlFileVersion() { nMajor = nMinor = 0; }
	int nMajor, nMinor;
};

class DTableInfo
{
public:
	DTableInfo();
	~DTableInfo();
	
	bool Parse(const tinyxml2::XMLNode *element, std::string *errMsg);

	const tinyxml2::XMLNode *ptrElement;

	std::string name;
	const tinyxml2::XMLAttribute* nameAttr;

	bool isPartial;
	const tinyxml2::XMLAttribute* isPartialAttr;

	bool isAllowedPartialTable;
	const tinyxml2::XMLAttribute* isAllowedPartialTableAttr;

	sXmlFileVersion version;
	const tinyxml2::XMLAttribute* versionAttr;

	std::string remark;
	const tinyxml2::XMLAttribute* remarkAttr;

};
