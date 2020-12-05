#pragma once
#include "CSingletonBase.hpp"
#include "DTable.h"
#include <mutex>
struct sThreadArg;
class DXmlFile;

#define TABLE_XML_VERSION	1, 0, 0, 0

class CDataFactory : public CSingletonBase<CDataFactory>
{
	DECLARE_SINGLETON(CDataFactory)
	CDataFactory();
	~CDataFactory();

public:
	virtual void Destroy() override;
	bool Initialize(std::vector<std::string> xmlFiles, int coreCount, std::string *errMsg);
	
	std::vector<DXmlFile*> loadFiles;
	std::string errMsg;
	std::mutex mtx;
	static void LoadXmlThread(sThreadArg *arg);
	
	static bool CSVToXML(const char *csvPath, const char* xmlPath, std::string *errMsg);
};

