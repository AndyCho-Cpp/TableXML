#include "pch.h"
#include "DKeyInfo.h"
#include "tinyxml2_helper.hpp"
#include <map>
#include "CommonStringHelper.hpp"
#include "CommonTimeHelper.hpp"
#include "DColumnInfo.h"

using namespace CommonHelper;

DKeyInfo::DKeyInfo()
{
	ptrElement = nullptr; primaryKeyAttr = foreignKeysAttr = nullptr;
}


DKeyInfo::~DKeyInfo()
{
	DELETE_VECTOR_OBJECT(foreignKeys);
}

// ex) <Header primaryKey="order_id" foreignKey="customer_sid => customer(sid) update=cascade delete=restrict">
bool DKeyInfo::Parse(const tinyxml2::XMLNode* node, std::string *errMsg)
{
	ptrElement = node;

	tinyxml2::XMLElement* elem = (tinyxml2::XMLElement*)node;
	
	AttributeHash headerAttrs;
	GetAllAttributes_inOneNode(elem, &headerAttrs);

	auto itrPri = headerAttrs.find("primaryKey");
	if (itrPri == headerAttrs.end())
	{
		*errMsg = "The [primaryKey] attribute must be defined in <Header> element.";
		return false;
	}

	for (auto &itr : headerAttrs)
	{
		if (itr.first == "primaryKey")
		{
			this->primaryKeyAttr = itr.second.ptr;
			std::vector<std::string> priKeys = StrSplit(itr.second.value, ',', true);
			for (auto ii : priKeys)
				this->primaryKeys.push_back(ii);
		}
		else if (itr.first == "foreignKey")
		{
			this->foreignKeysAttr = itr.second.ptr;
			std::vector<std::string> fKeys = StrSplit(itr.second.value, ',', true);
			for (auto ii : fKeys)
			{
				sForeignKey *fk = new sForeignKey;
				std::string ee;
				if (ParseForeignKey(fk, ii.c_str(), &ee) == false)
				{
					*errMsg += StringFormat1KB("The [foreignKey] attribute parsing failure in <Header> element => %s\n", ee.c_str());
					return false;
				}
				this->foreignKeys.push_back(fk);
			}
		}
	}
	return true;
}

//ex) "customer_sid => customer(sid) update=cascade delete=restrict"
bool DKeyInfo::ParseForeignKey(sForeignKey *target, const char *text, std::string *errMsg)
{
	std::vector<std::string> v = StrSplit(text, ' ', true);
	if (v.size() < 3)
	{
		// col => table(col)  3요소 불충분
		return false;
	}
	if (v[1] != "=>")
	{
		// not found =>
		return false;
	}

	target->name = v[0];
	std::vector<std::string> ff = StrSplit(v[2], '(', true);
	v.erase(v.begin(), v.begin() + 3); // erase 0~2
	if (ff.size() != 2)
	{
		// refer table and col name wrong
		return false;
	}
	if (ff[1].size() < 2)
	{
		// colName) 뒤에 괄호 닫는것 때문에 반드시 2자 이상
		return false;
	}
	if (ff[1].at(ff[1].size() - 1) != ')')
	{
		// colName) 마지막에 괄호 닫기가 아님
		return false;
	}
	ff[1].erase(ff[1].begin() + ff[1].size() - 1);
	target->referTableName = ff[0];
	target->referTableColName = ff[1];

	// option parsing
	for (auto itr : v)
	{
		std::vector<std::string> v_opt = StrSplit(itr, '=', true);
		if (v_opt.size() != 2)
		{
			// 옵션명=옵션값  방식
			return false;
		}
		if (v_opt[0] == "update")
		{
			if (v_opt[1] == "cascade")
				target->updateType = eForeignKeyType::eCascade;
			else if (v_opt[1] == "restrict")
				target->updateType = eForeignKeyType::eRestric;
			else
			{
				return false;
			}
		}
		else if (v_opt[0] == "delete")
		{
			if (v_opt[1] == "cascade")
				target->deleteType = eForeignKeyType::eCascade;
			else if (v_opt[1] == "restrict")
				target->deleteType = eForeignKeyType::eRestric;
			else
			{
				return false;
			}
		}
		else
		{
			// not support in this version
		}
	}
	return true;
}

bool DKeyInfo::SetKeyObject(const std::map<std::string, DColumnInfo*> &hash)
{
	for (auto &itr_col : hash)
	{
		DColumnInfo* col = itr_col.second;

		for (auto &key : primaryKeys)
		{
			if (col->name == key)
			{
				primaryKeyObjHash[key] = col;
				col->isPrimaryKey = true;
			}
		}
		for (auto fKey : foreignKeys)
		{
			foreignKeyObjHash[fKey->name] = col;
			col->ptrForeignKey = fKey;
		}
	}
	return true;
}
