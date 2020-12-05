#pragma once
#include <string>
#include <map>
#include <unordered_map>
#include "DTableInfo.h"
#include "DKeyInfo.h"

namespace tinyxml2
{
	class XMLDocument;
	class XMLNode;
	class XMLAttribute;
	class XMLElement;
	//enum XMLError;
};

class DColumnInfo;
class DRowInfo;


class DXmlFile
{
public:
	DXmlFile();
	~DXmlFile();

	DTableInfo tableInfo;
	DKeyInfo keyInfo;

	bool LoadXml(const char *filePath, std::string *errMsg);
	bool SaveXml(std::string *errMsg);

	std::map<std::string/*col name*/, DColumnInfo*> cols_hash;

	bool ParseColumns(tinyxml2::XMLElement* columNode, std::string *errMsg);
	bool ParseRows(tinyxml2::XMLElement*, std::string *errMsg);

	std::unordered_map<std::string/*primary key*/, DRowInfo*> rowsHash;

	std::string fileName;
	std::string fullPath;

	
	bool isLoaded;
	bool isValid;

	tinyxml2::XMLDocument *doc;
	int saveCount;

private:
	static DColumnInfo* GetColumnObject(std::string colName);
	bool MakeKey(DRowInfo* rowInfo, std::string *out_key, std::string *errMsg);
};


