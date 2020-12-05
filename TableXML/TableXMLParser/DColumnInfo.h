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

enum class eDataType : char { eInt, eDouble, eBool, eDate, eString, eChar };
struct sForeignKey;

class DColumnInfo
{
public:
	DColumnInfo();
	~DColumnInfo();
	
	
	bool Parse(const tinyxml2::XMLNode* node, std::string *errMsg);
	void SetIndex(int idx);
	bool isPrimaryKey;
	sForeignKey *ptrForeignKey; // null if this is not a foreign key
	

	const tinyxml2::XMLNode* ptrElement;

	std::string name;
	const tinyxml2::XMLAttribute *nameAttr;

	eDataType type;
	const tinyxml2::XMLAttribute *typeAttr;

	int length;
	const tinyxml2::XMLAttribute *lengthAttr;

	std::string remark;
	const tinyxml2::XMLAttribute *remarkAttr;

	
private:
	int index;
};

