#include "pch.h"
#include "DXmlFile.h"
#include "tinyxml2_helper.hpp"
#include "CommonStringHelper.hpp"
#include "CommonTimeHelper.hpp"
#include "DColumnInfo.h"
#include "DRowInfo.h"
#define UNIT_SEPARATOR 0x1F  //ascii code US
using namespace CommonHelper;

DXmlFile::DXmlFile()
{
	isLoaded = false;
	isValid = false;\
	doc = nullptr;
}


DXmlFile::~DXmlFile()
{
	DELETE_OBJECT(doc);
	DELETE_MAP_OBJECT(cols_hash);
}

bool DXmlFile::LoadXml(const char *filePath, std::string *errMsg)
{
	if (isLoaded)
	{
		if (errMsg != nullptr)
			*errMsg = StringFormat1KB("The file is already loaded.");
		return false;
	}
	if (IsFileExists(filePath) == false)
	{
		if (errMsg != nullptr)
			*errMsg = StringFormat1KB("The fIle does not exist.");
		return false;
	}

	fullPath = filePath;
	fileName = ExtractFileName(fullPath);

	if (doc != nullptr)
		delete doc;

	doc = new tinyxml2::XMLDocument;

	tinyxml2::XMLError error = tinyxml2::XMLError::XML_SUCCESS;
	constexpr int openRetry_ms = 3000;
	TimePoint tp = StdClock::Now_t();
	do
	{
		error = doc->LoadFile(filePath);
		if (tinyxml2::XMLError::XML_SUCCESS == error)
			break;	
		Sleep(100);	
	} while (StdClock::Elapsed_ms(tp) < openRetry_ms);

	if (error != tinyxml2::XMLError::XML_SUCCESS)
	{
		delete doc;
		doc = nullptr;
		if (errMsg != nullptr)
			*errMsg = StringFormat1KB("XML Load Failure. TinyXML Error: %s(%d)", TinyErrorToString(error), error);
		return false;
	}

	// Load Basic Information * * * * * * * *
	//  such as table name,  parital info, version etc...
	//
	tinyxml2::XMLNode *rootNode = doc->RootElement();
	if (rootNode == nullptr)
	{
		*errMsg = "Empty XML.";
		return false;
	}
	tinyxml2::XMLElement* elem = (tinyxml2::XMLElement*)rootNode;
	if (IsStringEqual(elem->Name(), "TableXML") == false)
	{
		*errMsg = "Cannot find <TableXML> element.";
		return false;
	}

	std::string err;
	if (tableInfo.Parse(elem, &err) == false)
	{
		return false;
	}

	tinyxml2::XMLElement* columNode = nullptr;
	tinyxml2::XMLElement* rowsNode = nullptr;

	for (tinyxml2::XMLNode* child = (tinyxml2::XMLNode*)rootNode->FirstChildElement(); child != nullptr; child = child->NextSiblingElement())
	{
		tinyxml2::XMLElement* elem = (tinyxml2::XMLElement*)child;

		if (IsStringEqual(elem->Name(), "Header"))
		{
			if (tableInfo.isPartial)
			{
				*errMsg = "PartialTable is not allowed to define header information.";
				return false;
			}

			columNode = elem;

			if (keyInfo.Parse(elem, &err) == false)
			{
				return false;
			}
		}
		else if (IsStringEqual(elem->Name(), "Rows"))
		{
			rowsNode = elem; // head of row node
		}
		else
		{
			*errMsg = StringFormat1KB("Command not supported => <%s>", elem->Name());
			return false;
		}
	}

	if (ParseColumns(columNode, errMsg) == false)
	{
		return false;
	}

	keyInfo.SetKeyObject(cols_hash);

	if (ParseRows(rowsNode, errMsg) == false)
	{
		return false;
	}


	isLoaded = true;
	return true;
}

bool DXmlFile::ParseColumns(tinyxml2::XMLElement* columNode, std::string *errMsg)
{
	int n = 0;
	for (tinyxml2::XMLNode* child = (tinyxml2::XMLNode*)columNode->FirstChildElement(); child != nullptr; child = child->NextSiblingElement())
	{
		tinyxml2::XMLElement* elem = (tinyxml2::XMLElement*)child;

		if (IsStringEqual(elem->Name(), "Column") == false)
		{
			*errMsg = StringFormat1KB("Wrong element <%s>. This must be <Column>", elem->Name());
			return false;
		}

		DColumnInfo *pCol = new DColumnInfo;
		std::string ee;
		if (pCol->Parse(elem, &ee) == false)
		{
			*errMsg = StringFormat1KB("Column parsing failure => %s", ee.c_str());
			delete pCol;
			return false;
		}
		pCol->SetIndex(n++);

		if (cols_hash.find(pCol->name) != cols_hash.end())
		{
			*errMsg = StringFormat1KB("Duplicated column => %s", pCol->name.c_str());
			delete pCol;
			pCol = nullptr;
			return false;
		}

		if (pCol) // insert into hash table
		{
			cols_hash[pCol->name] = pCol;
		}
	}

	return errMsg->size() == 0;
}

bool DXmlFile::ParseRows(tinyxml2::XMLElement* elem, std::string *errMsg)
{
	int n = 0;
	for (tinyxml2::XMLNode* child = (tinyxml2::XMLNode*)elem->FirstChildElement(); child != nullptr; child = child->NextSiblingElement())
	{
		tinyxml2::XMLElement* elem = (tinyxml2::XMLElement*)child;

		if (IsStringEqual(elem->Name(), "Row") == false)
		{
			*errMsg = StringFormat1KB("Wrong element <%s>. This must be <Row>", elem->Name());
			return false;
		}

		DRowInfo *pRow = new DRowInfo;
		std::string ee;
		if (pRow->Parse(&cols_hash, elem, &ee) == false)
		{
			*errMsg = StringFormat1KB("Row parsing failure => %s", ee.c_str());
			delete pRow;
			return false;
		}

		std::string key;
		if (MakeKey(pRow, &key, errMsg) == false)
		{
			delete pRow;
			return false;
		}

		rowsHash[key] = pRow;
	}
	return true;
}


bool DXmlFile::SaveXml(std::string *errMsg)
{
	if (isLoaded == false)
	{
		if (errMsg != nullptr)
			*errMsg = "File is not loaded.";
		return false;
	}
	if (doc == nullptr)
	{
		if (errMsg != nullptr)
			*errMsg = "XML is not loaded.";
		return false;
	}

	tinyxml2::XMLError error = tinyxml2::XMLError::XML_SUCCESS;
	constexpr int saveRetry_ms = 3000;

	TimePoint tp = StdClock::Now_t();
	do
	{
		error = doc->SaveFile(fullPath.c_str());
		if (tinyxml2::XMLError::XML_SUCCESS == error)
			break;
		Sleep(100);
	} while (StdClock::Elapsed_ms(tp) < saveRetry_ms);

	if (error != tinyxml2::XMLError::XML_SUCCESS)
	{
		delete doc;
		doc = nullptr;
		if (errMsg != nullptr)
			*errMsg = StringFormat1KB("XML Save Failure. File: [%s], Error: %s(%d)", fileName.c_str(), TinyErrorToString(error), error);
		return false;
	}
	saveCount++;
	return true;
}

DColumnInfo* DXmlFile::GetColumnObject(std::string colName)
{
	return nullptr;
}

bool DXmlFile::MakeKey(DRowInfo* rowInfo, std::string *out_key, std::string *errMsg)
{
	std::string sum;
	for (std::string &key : keyInfo.primaryKeys)
	{
		auto itr = rowInfo->subItems.find(key);
		if (itr == rowInfo->subItems.end())
		{
			*errMsg = StringFormat1KB("The key [%s] attribute is not exists in a row line number [%d]", key.c_str(), rowInfo->element ? rowInfo->element->GetLineNum() : -1);
			return false;
		}
		if (IsEmptyString(itr->second->value))
		{
			*errMsg = StringFormat1KB("The key [%s] attribute is empty in a row line number [%d]", key.c_str(), rowInfo->element ? rowInfo->element->GetLineNum() : -1);
			return false;
		}
		sum += itr->second->value + (char)UNIT_SEPARATOR;
	}
	auto itr = rowsHash.find(sum);
	if (itr != rowsHash.end())
	{
		*errMsg = StringFormat1KB("Duplicated key in a row line number [%d] => [%d]", rowInfo->element->GetLineNum(), itr->second->element->GetLineNum());
		return false;
	}
	*out_key = sum;
	return true;
}
