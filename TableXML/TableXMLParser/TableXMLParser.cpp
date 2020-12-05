// TableXMLParser.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "pch.h"
#include <iostream>
#include "tinyxml2_helper.hpp"
#include <map>
#include <Windows.h>
#include <stdlib.h>
#include <vector>
#include "CDataFactory.h"
#include "CommonFunction.hpp"
#define IsFileExists(szPath)  (GetFileAttributes(szPath) != INVALID_FILE_ATTRIBUTES && !(GetFileAttributes(szPath) & FILE_ATTRIBUTE_DIRECTORY))

using namespace CommonHelper;
inline bool FindFileAndDir(const char *szSrcPath, const char *szKeyword, std::vector<std::string> *outList)
{
	WIN32_FIND_DATA fdFile;
	HANDLE hFind = NULL;

	char sPath[2048] = { 0 };

	sprintf_s(sPath, "%s\\%s", szSrcPath, szKeyword);

	if ((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE)
		return false;

	do
	{
		if (strcmp(fdFile.cFileName, ".") == 0 || strcmp(fdFile.cFileName, "..") == 0)
			continue;
		
		//Build up file path using the passed in
		sprintf_s(sPath, "%s\\%s", szSrcPath, fdFile.cFileName);
		if(outList != nullptr)
			outList->push_back(sPath);

	} while (FindNextFile(hFind, &fdFile));

	FindClose(hFind);
	return true;
}

/**
* @brief  Tour all file and directory in soruce path by recursive searching.
* @author  soohwan.cho
* @date  2020.09.06
* @remark  Windows only
* @param  const char *szSrcPath : input - source directory path
* @param  std::vector<std::string> input - _excludeList : FIle and directory list that exclude from result.
   ex) debug, bin, x86, *.exe, *.pdb
* @param  std::vector<std::string> input - includeExtension : FIle and directory that only include to the result.
  empty vector means include all
  ex) xml, txt ==> means xml, txt files only. Do not attach '*.' 
  ex) *.xml or *.txt (X) bad
  ex) xml, txt  (O) good
* @param  std::vector<std::string> *outFileList : output - Result file list 
* @param  std::vector<std::string> *outDirList : output - Result directory list
* @param  std::string *errMsg : output error message.
*/
inline bool DirectoryTour_Recursive(const char *szSrcPath, const std::vector<std::string> *_excludeList, const std::vector<std::string> *includeExtension, std::vector<std::string> *outFileList, std::vector<std::string> *outDirList, std::string *errMsg)
{
	WIN32_FIND_DATA fdFile;
	HANDLE hFind = NULL;

	std::vector<std::string> v_excludeList;
	if (_excludeList != nullptr)
	{
		for (auto &itr : *_excludeList)
		{
			FindFileAndDir(szSrcPath, itr.c_str(), &v_excludeList);
		}
	}
	char sPath[2048] = { 0 };
	sprintf_s(sPath, "%s\\*.*", szSrcPath); // *.*  every file mask

	if ((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE)
	{
		if (errMsg != nullptr)
		{
			if (errMsg->size() > 0)
				*errMsg += "\n";
			*errMsg += "Path not found: " + std::string(szSrcPath);
		}
		return false;
	}


	do
	{
		if (strcmp(fdFile.cFileName, ".") == 0 || strcmp(fdFile.cFileName, "..") == 0)
			continue;
		
		bool skip = false;

		if (_excludeList != nullptr)
		{
			// exclude directory check
			for (auto &itr : *_excludeList)
			{
				if (itr.size() == 0)
					continue;
				if (itr.at(0) == '*')
					continue;				
				if (_strcmpi(fdFile.cFileName, itr.c_str()) == 0)
				{
					skip = true;
					break;
				}
			}
			if (skip)
				continue;
		}
		//Build up file path using the passed in
		sprintf_s(sPath, "%s\\%s", szSrcPath, fdFile.cFileName);

		
		for (auto &itr : v_excludeList)  // exclude file check
		{
			if (sPath == itr)
			{
				skip = true;
				break;
			}
		}
		if (skip)
			continue;

		//Is the entity a File or Folder?
		if (fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if(outDirList != nullptr)
				outDirList->push_back(sPath);
			DirectoryTour_Recursive(sPath, _excludeList, includeExtension, outFileList, outDirList, errMsg); //Recursion
		}
		else 
		{
			bool isIncludeAll = includeExtension == nullptr ? true : includeExtension->size() == 0;

			if (isIncludeAll)
			{
				if (outFileList != nullptr)
					outFileList->push_back(sPath);
			}
			else
			{
				for (auto itr : *includeExtension)
				{
					char drive[_MAX_DRIVE] = { 0 };
					char dir[_MAX_DIR] = { 0 };
					char fname[_MAX_FNAME] = { 0 };
					char ext[_MAX_EXT] = { 0 };
					_splitpath_s(sPath, drive, dir, fname, ext);
					char *ext2 = nullptr;
					if (strlen(ext) > 0) // remove '.'
						ext2 = &ext[1];
					else
						ext2 = &ext[0];
					
					if (_strcmpi(itr.c_str(), ext2) == 0)
					{
						if (outFileList != nullptr)
							outFileList->push_back(sPath);
					}
				}
			}
		}
		
	} while (FindNextFile(hFind, &fdFile));

	FindClose(hFind);

	return true;
}

int main(int argc, char *argv[])
{
	std::string modulePath = CombinePath(GetRunDirPath(), "TableXML.ini");
	char szBaseDir[2048] = { 0 };
	char exclude[2048] = { 0 };
	char coreCnt[8] = { 0 };
	GetPrivateProfileString("TableXML", "BaseDir", "", szBaseDir, sizeof(szBaseDir), modulePath.c_str());
	GetPrivateProfileString("TableXML", "Exclude", "", exclude, sizeof(exclude), modulePath.c_str());
	GetPrivateProfileString("TableXML", "CoreCount", "", coreCnt, sizeof(coreCnt), modulePath.c_str());
	int nCoreCnt = 1;
	TryParseInt(coreCnt, &nCoreCnt);
	std::string path = strlen(szBaseDir) > 2 ? (szBaseDir[1] == ':' ? szBaseDir : CombinePath(GetRunDirPath(), szBaseDir)) : CombinePath(GetRunDirPath(), szBaseDir);


	std::vector<std::string> excludeList = StrSplit(exclude, ';', true);

	std::vector<std::string> xmlFiles;
	std::vector<std::string> includeList = {"xml"};
	std::string errMsg;
	bool ret = DirectoryTour_Recursive(path.c_str(), &excludeList, &includeList, &xmlFiles, nullptr, &errMsg);
	if (ret == false)
	{
		printf("Error: %s\n", errMsg.c_str());
		return 1;
	}
	if (errMsg.size() > 0)
	{
		printf("* * * Warning * * *\n%s\n", errMsg.c_str());
	}

	errMsg.clear();


	if (CDataFactory::CSVToXML("D:\\driver.csv", "D:\\driver.xml", &errMsg) == false)
	{
		printf("%s\n", errMsg.c_str());
		return 1;
	}
	return 0;

	CDataFactory::Inst().Initialize(xmlFiles, nCoreCnt, &errMsg);

	return 0;
}
