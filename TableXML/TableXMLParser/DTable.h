#pragma once
#include "DColumn.h"
#include "DDataRow.h"
#include <vector>
#include <unordered_map>
class DColumn;
class DDataRow;

class DTable
{
public:
	DTable();
	~DTable();

	bool Create(std::vector<std::string> xmlPath);  // xml file can be multiple. because of partial xml

	bool AddColumn(DColumn *colObj);
	DColumn *GetColumn(int colIndex);
	DColumn *GetColumn(const char *colName);
	int GetColumnCount();

private:



	std::vector<DColumn*> columns;
	std::unordered_map<std::string/*col name*/, int/*col index*/> column_hash;

	std::vector<DDataRow*> rows;
	std::unordered_map<std::string, DDataRow*> row_hash;

	std::string KeyMaker(std::vector<std::string>keys);
};

