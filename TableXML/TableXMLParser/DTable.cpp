#include "pch.h"
#include "DTable.h"


DTable::DTable()
{

}


DTable::~DTable()
{

}

// xml file can be multiple. because of partial xml
bool DTable::Create(std::vector<std::string> xmlPath)
{

	return true;
}

bool DTable::AddColumn(DColumn *colObj)
{
	return true;
}

DColumn *DTable::GetColumn(int colIndex)
{
	return nullptr;
}

DColumn *DTable::GetColumn(const char *colName)
{
	return nullptr;
}

int DTable::GetColumnCount()
{
	return 0;
}

std::string DTable::KeyMaker(std::vector<std::string>keys)
{
	return "";
}