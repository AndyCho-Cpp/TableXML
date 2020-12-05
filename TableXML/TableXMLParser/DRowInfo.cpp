#include "pch.h"
#include "DRowInfo.h"
#include "tinyxml2_helper.hpp"
#include <map>
#include "CommonStringHelper.hpp"
#include "CommonTimeHelper.hpp"
#include "DColumnInfo.h"
using namespace CommonHelper;
DRowInfo::DRowInfo()
{
}


DRowInfo::~DRowInfo()
{
	DELETE_MAP_OBJECT(subItems);

}

bool DRowInfo::Parse(const std::map<std::string, DColumnInfo*> *colHash, const tinyxml2::XMLNode* node, std::string *errMsg)
{
	element = node;

	tinyxml2::XMLElement* elem = (tinyxml2::XMLElement*)node;

	AttributeHash attrs;
	GetAllAttributes_inOneNode(elem, &attrs);

	for (auto &itr : attrs)
	{
		AttributeNodeEx *p = new AttributeNodeEx(itr.second);
		auto colItr = colHash->find(p->name);
		if (colItr == colHash->end())
		{
			*errMsg = StringFormat1KB("The attribute [%s] is not defined.", p->name.c_str());
			return false;
		}
		p->colInfo = colItr->second;
		subItems[itr.second.name] = p;
	}

	return true;
}

void DRowInfo::SetKey(std::string key)
{
	stringKey = key;
}