#include "pch.h"
#include "DTableInfo.h"
#include "tinyxml2_helper.hpp"
#include <map>
#include "CommonStringHelper.hpp"
#include "CommonTimeHelper.hpp"

using namespace CommonHelper;

DTableInfo::DTableInfo()
{
	ptrElement = nullptr;
	isPartial = isAllowedPartialTable = false;
	nameAttr = isPartialAttr = isAllowedPartialTableAttr = versionAttr = remarkAttr = nullptr;
}


DTableInfo::~DTableInfo()
{

}

bool DTableInfo::Parse(const tinyxml2::XMLNode *node, std::string *errMsg)
{
	ptrElement = node;

	tinyxml2::XMLElement* elem = (tinyxml2::XMLElement*)node;
	
	AttributeHash attributesHash;
	GetAllAttributes_inOneNode(elem, &attributesHash);

	auto itr = attributesHash.find("name");  // table name
	if (itr == attributesHash.end())
	{
		*errMsg = "The [name] attribute is not defined in <TableXML>.";
		return false;
	}

	this->nameAttr = itr->second.ptr;
	this->name = itr->second.value;

	itr = attributesHash.find("isPartialTable");
	if (itr != attributesHash.end())
	{
		this->isPartialAttr = itr->second.ptr;
		TryParseBool(itr->second.value.c_str(), &this->isPartial);
	}
	else
	{
		*errMsg = "The [isPartialTable] attribute is not defined in <TableXML> element.";
		return false;
	}

	itr = attributesHash.find("allowPartialTable");
	if (itr != attributesHash.end())
	{
		this->isAllowedPartialTableAttr = itr->second.ptr;
		TryParseBool(itr->second.value.c_str(), &this->isAllowedPartialTable);
	}

	itr = attributesHash.find("version");
	
	if (itr != attributesHash.end())
	{
		this->versionAttr = itr->second.ptr;
		std::vector<std::string> v_version = StrSplit(itr->second.value, '.'); // version="x.x"
		if (v_version.size() >= 2)
		{
			TryParseInt(v_version[0].c_str(), &this->version.nMajor);
			TryParseInt(v_version[1].c_str(), &this->version.nMinor);
		}
	}

	itr = attributesHash.find("remark");

	if (itr != attributesHash.end())
	{
		this->remarkAttr = itr->second.ptr;
		this->remark = itr->second.value;
	}
	return true;
}