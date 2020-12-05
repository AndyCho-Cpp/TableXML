/**
* @author  soohwan.cho
* @date  2018.04.16
* @breif  All code was developed by soohwan.cho
*/

#pragma once
#include "CommonDefine.hpp"
#include "CommonStringHelper.hpp"
#include "CommonUtilites.hpp"
#include <windows.h>
#include <vector>

#ifndef UNDER_RTSS
#include <fstream>
#include <mutex>
#else
#include "esLib.h"
#pragma comment(lib, "esLib.lib")
#ifndef _DEBUG
	#pragma comment (lib, "esLib.lib")
#else
	#pragma comment (lib, "esLib_d.lib")
#endif
#endif

namespace CommonHelper
{
	// --------------------------------------------------------------------------------------------------------------------------------------
	// File Function
	//
	inline void RapidLog(const char *pFilePath_withoutExt, const char *pStr, bool bTime = true, bool bTimeOnFileName = true);
	inline void RapidLog2(const char *pFilePath_withoutExt, bool isErrorLog, const char *func, int line, const char* format, ...);
	class SHFile;
	// --------------------------------------------------------------------------------------------------------------------------------------

	/**
	* @breif: Rapid Log. write log right away. no queueing.
	* @author: soohwan.cho(@semes.com)
	* @argument: pFilePath_withoutExt : file path
	* bTime : whether write time
	* bTimeOnFileName : whether add date time at file name
	* 		ex) input: "TestLog" ==> "TestLog2018072316.log"
	*/
	inline void RapidLog(const char *pFilePath_withoutExt, const char *pStr, bool bTime/* = true*/, bool bTimeOnFileName/* = true*/)
	{
		FILE *fp = NULL;
		
#ifndef UNDER_RTSS
		static std::mutex mtx;
		std::lock_guard<std::mutex> lock(mtx);
#else
		static esLib::esMutex mtx;
		static bool bOnce = true;
		if(bOnce)
		{
			bOnce = false;
			mtx.Open();
		}
		
		mtx.Lock();
		esLib::esScopeLock lock(mtx);
#endif

		try
		{
			SYSTEMTIME CurTime;
			GetLocalTime(&CurTime);

			char szFilePath[512];
			memset(szFilePath, NULL, sizeof(szFilePath));

			if (bTimeOnFileName)   	sprintf_s(szFilePath, "%s%d%02d%02d%02d.log", pFilePath_withoutExt, CurTime.wYear, CurTime.wMonth, CurTime.wDay, CurTime.wHour);
			else             		sprintf_s(szFilePath, "%s.log", pFilePath_withoutExt);
		
			fopen_s(&fp, szFilePath, "a");    // write+append
			if (fp == NULL)
			{
				char drive[_MAX_DRIVE] = { 0 };
				char dir[_MAX_DIR] = { 0 };
				char fname[_MAX_FNAME] = { 0 };
				char ext[_MAX_EXT] = { 0 };
				_splitpath_s(szFilePath, drive, dir, fname, ext);
				std::string dir_ = std::string(drive) + dir;
				CreateDirectoryDeeply(dir_.c_str());
				
				fopen_s(&fp, szFilePath, "a");    // write+append
				if (fp == nullptr)
					return;
			}
		

			if (bTime)
			{
				fprintf(fp, "[%02d:%02d:%02d.%03d] ",
					CurTime.wHour, CurTime.wMinute, CurTime.wSecond, CurTime.wMilliseconds);
			}
			fwrite(pStr, 1, strlen_i(pStr), fp);
			fwrite("\n", 1, 1, fp);
			fclose(fp);

		}
		catch (...)
		{
			if (fp != NULL)
				fclose(fp);
		}
	}


	inline void RapidLog2(const char *pFilePath_withoutExt, bool isErrorLog, const char *func, int line, const char* format, ...)
	{
		char buf[1024];
		memset(buf, NULL, sizeof(buf));

		try
		{
			va_list args;

			va_start(args, format);
			vsnprintf_s(buf, sizeof(buf), sizeof(buf) - 1, format, args);
			va_end(args);
		}
		catch (...) {}


		if (isErrorLog)
			RapidLog(pFilePath_withoutExt, StringFormat1KB("[ERROR] %s()[%d], %s", func, line, buf).c_str());
		else
			RapidLog(pFilePath_withoutExt, StringFormat1KB("%s()[%d], %s", func, line, buf).c_str());
		
	}
	
	// soohwan file helper
	class SHFile
	{
	public:
		/**
		* @brief: read all texts from file. return with string
		* @author: soohwan.cho(@semes.com)
		* @ps: keep author define comment above
		*/
		inline static std::string ReadAllTexts(std::string filepath)
		{
			FILE* fp = nullptr;
			fopen_s(&fp, filepath.c_str(), "rb");
			if (fp == nullptr)
				return "";

			fseek(fp, 0, SEEK_END);	// get file size : go to end of file
			int fileSize = ftell(fp);
			fseek(fp, 0, SEEK_SET);	// go to start pos


			char* buf = new (std::nothrow) char[fileSize + 1];
			if (buf == nullptr)
				return "";

			int filePtr = 0;
			int needToRead = fileSize;

			while (needToRead > 0)
			{
				int readByte = fread(buf + filePtr, 1, __MIN__(needToRead, 8192), fp);
				if (readByte == 0)
					break;
				filePtr += readByte;
				needToRead -= readByte;
			}

			buf[fileSize] = '\0';
			fclose(fp);

			if (fileSize != filePtr)
				return "";

			return buf;
		}
		/**
		* @brief: read all lines from file. return with string vector
		* @author: soohwan.cho(@semes.com)
		* @ps: keep author define comment above
		*/
		inline static void ReadAllLines(std::string filepath, std::vector<std::string>& lines)
		{
			std::string contents = ReadAllTexts(filepath);;
			if (contents == "")
				return;

			CommonHelper::StrSplitEx(contents, '\n', lines, true);
		}

		inline static bool WriteText(std::string &filePath, std::string &text, bool append = true)
		{
			FILE* fp = nullptr;
			fopen_s(&fp, filePath.c_str(), append ? "a+" : "wb");
			if (fp == nullptr)
				return false;

			const char* buf = text.c_str();
			int needToWrite = text.size();
			int filePtr = 0;

			while (needToWrite > 0)
			{
				int wb = fwrite(buf + filePtr, 1, __MIN__(needToWrite, 8192), fp);
				if (wb == 0)
					break;
				filePtr += wb;
				needToWrite -= wb;
			}
			fclose(fp);
			return filePtr == text.size();
		}
	};
};;