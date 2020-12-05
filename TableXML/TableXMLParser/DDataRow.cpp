#include "pch.h"
#include "DDataRow.h"


DDataRow::DDataRow(DTable *parent)
{
	this->parent = parent;
	this->subItems = new std::string[parent->GetColumnCount()];
}


DDataRow::~DDataRow()
{

}

std::string &DDataRow::GetData(int colIndex)
{
	return subItems[colIndex];
}

std::string &DDataRow::GetData(const char* colName)
{
	return GetData(parent->GetColumn(colName)->colNo);
}

bool DDataRow::SetData(int colIndex, std::string &val, std::string *errMsg)
{
	subItems[colIndex] = val;
	// Queue Push!
	return true;
}

bool DDataRow::SetData(const char* colName, std::string &val, std::string *errMsg)
{
	return SetData(parent->GetColumn(colName)->colNo, val, errMsg);
}

int DDataRow::ColCount()
{
	return parent->GetColumnCount();
}