#include "pch.h"
#include "DColumnInfo.h"
#include "tinyxml2_helper.hpp"
#include <map>
#include "CommonStringHelper.hpp"
#include "CommonTimeHelper.hpp"


using namespace CommonHelper;


DColumnInfo::DColumnInfo()
{
	index = 0;
	isPrimaryKey = false;
	type = eDataType::eInt;
	length = 0;
	ptrElement = nullptr;
	nameAttr = typeAttr = lengthAttr = remarkAttr = nullptr;
	ptrForeignKey = nullptr;
}


DColumnInfo::~DColumnInfo()
{
}
void DColumnInfo::SetIndex(int idx)
{
	this->index = idx;
}

// ex) <Column name="customer_sid" type="INTEGER" length="5"/>
bool DColumnInfo::Parse(const tinyxml2::XMLNode* node, std::string *errMsg)
{
	ptrElement = node;

	tinyxml2::XMLElement* elem = (tinyxml2::XMLElement*)node;

	AttributeHash attrs;
	GetAllAttributes_inOneNode(elem, &attrs);

	for (auto &itr : attrs)
	{
		if (itr.first == "name")
		{
			this->name = itr.second.value;
			this->nameAttr = itr.second.ptr;
		}
		else if (itr.first == "type")
		{
			this->typeAttr = itr.second.ptr;
			if (itr.second.value == "INTEGER")
				this->type = eDataType::eInt;
			else if (itr.second.value == "DOUBLE")
				this->type = eDataType::eDouble;
			else if (itr.second.value == "BOOL")
				this->type = eDataType::eBool;
			else if (itr.second.value == "DATE")
				this->type = eDataType::eDate;
			else if (itr.second.value == "STRING")
				this->type = eDataType::eString;
			else if (itr.second.value == "CHAR")
				this->type = eDataType::eChar;
			else
			{
				*errMsg = StringFormat1KB("<%s> Unknown data type [%s]", this->name.c_str(), itr.second.value.c_str());
				return false;
			}
		}
		else if (itr.first == "length")
		{
			this->lengthAttr = itr.second.ptr;
			if (TryParseInt(itr.second.value.c_str(), &this->length) == false)
			{
				*errMsg = StringFormat1KB("<%s> length attribute is not integer => %s", this->name.c_str(), itr.second.value.c_str());
				return false;
			}
		}
		else if (itr.first == "remark")
		{
			this->remarkAttr = itr.second.ptr;
			this->remark = itr.second.value;
		}
		else
		{
			// not support in this version
			continue;
		}
	}

	if (this->name.empty())
	{
		*errMsg = "Cannot find column name.";
		return false;
	}
	
	return true;
}