#pragma once
#include <string>
#include "DTable.h"

class DTable;
class DDataRow
{
public:
	DDataRow(DTable *parent);
	~DDataRow();
	std::string &GetData(int colIndex);
	std::string &GetData(const char* colName);
	bool SetData(int colIndex, std::string &val, std::string *errMsg);
	bool SetData(const char* colName, std::string &val, std::string *errMsg);
	int ColCount();

private:
	DTable *parent;
	std::string *subItems;
};

