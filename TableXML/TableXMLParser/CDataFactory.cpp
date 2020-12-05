#include "pch.h"
#include "CDataFactory.h"
#include "tinyxml2_helper.hpp"
#include "CommonFunction.hpp"
#include "DXmlFile.h"
//#include "CommonFunction.hpp"
#include <thread>
#include <queue>
using namespace CommonHelper;


IMPLEMENT_SINGLETON(CDataFactory)

struct sThreadArg
{
	sThreadArg() { kill = false; }
	std::queue<std::string/*xml file path*/> loadQueue;
	std::mutex mtx;
	std::thread *pThread;
	bool kill;
};


void CDataFactory::LoadXmlThread(sThreadArg *arg)
{
	while (arg->kill == false)
	{
		if (arg->loadQueue.size() == 0)
			break;
		
		std::string path;
		{	std::lock_guard<std::mutex> lock(arg->mtx);
			path = arg->loadQueue.front();
			arg->loadQueue.pop();
		}
		
		DXmlFile *pObj = new DXmlFile;
		std::string errMsg;

		// load xml
		if (pObj->LoadXml(path.c_str(), &errMsg) == false)
		{
			{	std::lock_guard<std::mutex> lock(CDataFactory::m_instance->mtx);
				CDataFactory::m_instance->errMsg += errMsg + "\n";
			}
			delete pObj;
			continue;
		}

		{	std::lock_guard<std::mutex> lock(CDataFactory::m_instance->mtx);
			CDataFactory::m_instance->loadFiles.push_back(pObj);
		}
	}
}

CDataFactory::CDataFactory()
{

}


CDataFactory::~CDataFactory()
{

}

void CDataFactory::Destroy()
{

}

bool CDataFactory::Initialize(std::vector<std::string> xmlFiles, int coreCount, std::string *errMsg)
{
	coreCount = __MIN__(16, __MAX__(1, coreCount)); // 1~16 core only
	if (coreCount > xmlFiles.size())
		coreCount = xmlFiles.size();

	sThreadArg *pThreadArg = new sThreadArg[coreCount];
	int i = 0;

	// divide jobs
	for (auto &file : xmlFiles)
		pThreadArg[i%coreCount].loadQueue.push(file);
	
	// run multiple thread to load xml files
	for (int k = 0; k < coreCount; k++)
	{
		std::thread *th = new std::thread(LoadXmlThread, &pThreadArg[k]);
		pThreadArg[k].pThread = th;
	}

	// wait thread for join
	for (int k = 0; k < coreCount; k++)
	{
		pThreadArg[k].pThread->join();
		delete pThreadArg[k].pThread;
	}
	
	return true;
}




bool CDataFactory::CSVToXML(const char *csvPath, const char* xmlPath, std::string *errMsg)
{
	std::vector<std::string> lines;
	try
	{
		tinyxml2::XMLDocument *doc = new tinyxml2::XMLDocument;
		tinyxml2::XMLElement *rootNode = doc->NewElement("TableXML");
		doc->InsertFirstChild(rootNode);

		rootNode->SetAttribute("name", ExtractFileName(csvPath, true).c_str());
		rootNode->SetAttribute("isPartialTable", "false");
		rootNode->SetAttribute("allowPartialTable", "false");
		rootNode->SetAttribute("version", StringFormat1KB("%d.%d.%d.%d", TABLE_XML_VERSION).c_str());
		
		tinyxml2::XMLElement *colHeaderNode = doc->NewElement("Header");
		tinyxml2::XMLElement *rowParentNode = doc->NewElement("Rows");
		
		rootNode->InsertFirstChild(colHeaderNode);
		rootNode->InsertAfterChild(colHeaderNode, rowParentNode);

		colHeaderNode->SetAttribute("primaryKey", "");
		colHeaderNode->SetAttribute("foreignKey", "");

		SHFile::ReadAllLines(csvPath, lines);
		int rowCnt = 0;

		if (lines.size() == 0)
		{
			*errMsg = "Empty File";
			return false;
		}

		std::string strColInfo = lines[0];
		lines.erase(lines.begin());
		std::vector<std::string> colItems;
		StrSplitEx(strColInfo, ',', colItems, true);
		tinyxml2::XMLNode *pCurrItem = nullptr;
		
		std::vector<std::string> cols;
		// parsing column header
		for (auto col : colItems)
		{
			do
			{
				// trim 머리부터 발끝까지
				if (col.size() > 0)
				{
					if (col.at(col.size() - 1) == '"')
					{
						col.erase(col.begin() + col.size() - 1);
						continue;
					}
				}
				if (col.size() > 0)
				{
					if (col.at(col.size() - 1) == '\r\n' || col.at(col.size() - 1) == '\n' || col.at(col.size() - 1) == '\r')
					{
						col.erase(col.begin() + col.size() - 1);
						continue;
					}
				}
				if (col.size() > 0)
				{
					if (col.at(0) == '"')
					{
						col.erase(col.begin());
						continue;
					}
				}
				break;
			} while (true);

			tinyxml2::XMLElement *colNode = doc->NewElement("Column");
			if (pCurrItem == nullptr)
				colHeaderNode->InsertFirstChild(colNode);
			else
				colHeaderNode->InsertAfterChild(pCurrItem, colNode);

			pCurrItem = colNode;
			colNode->SetAttribute("name", col.c_str());
			colNode->SetAttribute("type", "STRING");
			colNode->SetAttribute("length", "100");
			cols.push_back(col);
		}

		tinyxml2::XMLNode *pCurrRowItem = nullptr;
		for (auto &line : lines)
		{
			std::vector<std::string> items;
			StrSplitEx(line, ',', items, true);

			if (items.size() != cols.size())
			{
				*errMsg = StringFormat1KB("Column and row data count are mismatched. => %s", line.c_str());
				return false;
			}

			tinyxml2::XMLElement *rowNode = doc->NewElement("Row");
			if (pCurrRowItem == nullptr)
				rowParentNode->InsertFirstChild(rowNode);
			else
				rowParentNode->InsertAfterChild(pCurrRowItem, rowNode);
			
			pCurrRowItem = rowNode;

			int colIdx = 0;
			for (auto item : items)
			{
				do
				{
					// trim 머리부터 발끝까지
					if (item.size() > 0)
					{
						if (item.at(item.size() - 1) == '"')
						{
							item.erase(item.begin() + item.size() - 1);
							continue;
						}
					}
					if (item.size() > 0)
					{
						if (item.at(item.size() - 1) == '\r\n' || item.at(item.size() - 1) == '\n' || item.at(item.size() - 1) == '\r')
						{
							item.erase(item.begin() + item.size() - 1);
							continue;
						}
					}
					if (item.size() > 0)
					{
						if (item.at(0) == '"')
						{
							item.erase(item.begin());
							continue;
						}
					}
					if (item == "\\N")
					{
						item = "";
					}
					break;
				} while (true);
				rowNode->SetAttribute(cols[colIdx].c_str(), item.c_str());
				colIdx++;
			}
			
			rowCnt++;
		}

		auto ret = doc->SaveFile(xmlPath);
		delete doc;
		if (ret != XML_SUCCESS)
		{
			*errMsg = StringFormat1KB("XML Save Error Code: %d", (int)ret);
			return false;
		}
		
	}
	catch (const std::exception& exp)
	{

	}
	catch (...)
	{

	}
	return true;
}