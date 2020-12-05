#pragma once
#include <vector>
#include <string>
#include <map>
namespace tinyxml2
{
	class XMLDocument;
	class XMLNode;
	class XMLAttribute;
	class XMLElement;
	//enum XMLError;
};

enum class eForeignKeyType : char { eCascade, eRestric };

struct sForeignKey
{
	std::string name;
	std::string referTableName;
	std::string referTableColName;
	eForeignKeyType updateType;
	eForeignKeyType deleteType;
		
};

class DColumnInfo;

class DKeyInfo
{
public:
	DKeyInfo();
	~DKeyInfo();
	
	bool Parse(const tinyxml2::XMLNode* node, std::string *errMsg);
	bool SetKeyObject(const std::map<std::string, DColumnInfo*> &hash);
	const tinyxml2::XMLNode* ptrElement;

	std::vector<std::string> primaryKeys;
	const tinyxml2::XMLAttribute* primaryKeyAttr;
	std::map<std::string, DColumnInfo*> primaryKeyObjHash;

	std::vector<sForeignKey*> foreignKeys;
	const tinyxml2::XMLAttribute* foreignKeysAttr;
	std::map<std::string, DColumnInfo*> foreignKeyObjHash;
	

private:
	bool ParseForeignKey(sForeignKey *target, const char *text, std::string *errMsg);
};

