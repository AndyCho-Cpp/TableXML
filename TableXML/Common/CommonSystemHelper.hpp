/**
* @author  soohwan.cho
* @date  2018.04.16
* @breif  All code was developed by soohwan.cho
*/


#pragma once

#include "CommonDefine.hpp"
#include "CommonStringHelper.hpp"
#include <windows.h>
#include <Tlhelp32.h>
#include <shellapi.h>
#include <mutex>
#include <vector>
#include <fstream>
#pragma comment(lib, "Version.lib")

//#pragma warning(disable : 4267)

/**
* @author: soohwan.cho(@semes.com)
* @brief: sendHeader file only. common help function, class, define, etc...
* @date : 2018.04.16
* @ps: if you think this is a lots of helps on your project... call donate()
*/
namespace CommonHelper
{
	struct sEthernetInfo;
	struct sFileViersionInfoEx;

	// --------------------------------------------------------------------------------------------------------------------------------------
	// System Function
	//
	inline std::vector<std::string> GetCommandArgment();
	inline int GetEternetInfo(sEthernetInfo *arrEternetInfo, int nArrCnt);
	inline int GetCurrentThreadCount();
	inline bool GetDLLVersion(const char *filename, sFileViersionInfoEx *pVersion);
	inline bool GetProcessInfoByName(std::string szExeName, LPPROCESSENTRY32 out_processInfo = nullptr);
	inline bool TerminateProcessByName(std::string szExeName, int retryCnt = 5, int retryInterval_ms = 100);
	inline std::string GetLastErrorMessage(DWORD errorCode);
	// ----------------------------------------------------------------------------------------------------------------------------------------


	// GetEternetInfo(..) using
	struct sEthernetInfo
	{
		char Name[50];
		char IP[50];
		char SubnetMask[50];
		char Gateway[50];

		sEthernetInfo() { memset(this, NULL, sizeof(*this)); }
	};

	struct sFileViersionInfoEx
	{
		sFileViersionInfoEx() { memset(this, NULL, sizeof(*this)); }
		int major, minor, build, branch;
		std::string ToString() { return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(build) + "." + std::to_string(branch); }
	};

	/**
	* @brief: getting command argument
	* wide-char input command okay
	* @author: soohwan.cho(@semes.com)
	*/
	inline std::vector<std::string> GetCommandArgment()
	{
		int num = 0;

		std::vector<std::string> v_argv;

		int i;
		LPWSTR cmdLine = GetCommandLineW();
		LPWSTR* argv = CommandLineToArgvW(cmdLine, &num);

		for (i = 0; i < num; i++)
		{
			char strMultibyte[256] = { 0, };
			int len = WideCharToMultiByte(CP_ACP, 0, argv[i], -1, NULL, 0, NULL, NULL);
			WideCharToMultiByte(CP_ACP, 0, argv[i], -1, strMultibyte, len, NULL, NULL);
			v_argv.push_back(strMultibyte);
		}
		LocalFree(argv);
		return v_argv;
	}





	/**
	* @author: soohwan.cho(@semes.com) 2018.10.17  GetEternetInfo(...)
	* getting each ethernet card info.
	* "ipconfig" command parsing way. (Korean, Eng both are tested successfully.)
	* It's more useful than using the win-api whilch getting ethernet info
	*
	* @argement : arrEternetInfo => array for return.  nArrCnt=> count of input array
	* @return : found ethernet card count
	*/
	inline int GetEternetInfo(sEthernetInfo *arrEternetInfo, int nArrCnt)
	{
		const char tmpFilePath[] = "C:\\.network.tmp";
		DeleteFileA(tmpFilePath);   // 기존에 임시로 생성된게 혹시 있다면 삭제

		char szCmd[100];
		sprintf_s(szCmd, "ipconfig >> %s", tmpFilePath);   // ipconfig 돌린 임시 파일 생성


		system(szCmd); // run IPCONFIG
					   /// 아래 GetLocaleInfo 방식으로 세팅된 언어는 알아낼 수 있는데
					   /// 윈도우 설치 버전이 영문인지는 알수 없어서 페기함
					   ///TCHAR loc[80];
					   ///GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SENGLANGUAGE, loc, 80*sizeof(TCHAR));
					   ///GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SENGCOUNTRY, loc, 80*sizeof(TCHAR));

					   ///char szLog[80];
					   ///wcstombs(szLog, loc, sizeof(szLog));
		bool bKorean = false;	//strcmp(szLog, "Korean") == 0;

								// 각 sectino의 Key 이름변수. ex) IPV4주소, 서브넷마스크, 기본게이트웨이
		char szKeyName_Ethernet[50];
		char szKeyName_IP[50];
		char szKeyName_Subnet[50];
		char szKeyName_Gateway[50];

		// 윈도우 언어 알아내기. IPCONFIG 내용 보고 언어 판단
		{
			std::string tmp;
			std::ifstream tmpFile(tmpFilePath);
			while (std::getline(tmpFile, tmp))
			{
				int nPos = static_cast<int>(tmp.find("Windows IP 구성"));	// 영문은  Windows IP Configuration 이라고 나옴
				if (nPos >= 0)
				{
					bKorean = true;
					break;
				}
			}
			tmpFile.close();
			sprintf_s(szKeyName_Ethernet, bKorean ? "이더넷 어댑터" : "Ethernet adapter");
			sprintf_s(szKeyName_IP, bKorean ? "IPv4 주소" : "IP Address");
			sprintf_s(szKeyName_Subnet, bKorean ? "서브넷 마스크" : "Subnet Mask");
			sprintf_s(szKeyName_Gateway, bKorean ? "기본 게이트웨이" : "Default Gateway");
		}

		std::string line;
		std::ifstream infile(tmpFilePath);

		int nCurCnt = -1;
		bool bKnowLanguage = false;
		bool bIsEnglish = false;
		bool bIsKorean = false;
		// parsing.. line by line
		while (std::getline(infile, line))
		{
			if (line.length() < 3)
				continue;

			int nPos = static_cast<int>(line.find(szKeyName_Ethernet));
			if (nPos >= 0)
			{
				nCurCnt++;

				if (nCurCnt >= nArrCnt) // 입력 배열 개수 초과
					return nCurCnt + 1;

				// Ethernet adapter 로컬 영역 연결:
				// 위의 문자열에서 랜카드 이름 "로컬 영역 연결" 만 추출한다
				line.replace(0, nPos + strlen_i(szKeyName_Ethernet) + 1, "");
				if (line.find(":") >= 0)
					line.replace(line.find(":"), 1, "");

				strcpy_s(arrEternetInfo[nCurCnt].Name, line.c_str());
				continue;
			}
			if (nCurCnt < 0)
				continue;

			nPos = static_cast<int>(line.find(szKeyName_IP));
			if (nPos >= 0)
			{
				//영문 윈도 : IP Address. . . . . . . . . . . . : 100.100.100.100
				//한글 윈도 : IPv4 주소 . . . . . . . . . : 23.42.52.123
				//위의 문자열에서 ip 주소를 파싱한다

				int nColonPos = static_cast<int>(line.find(":", strlen_i(szKeyName_IP)));
				if (nColonPos < strlen_i(szKeyName_IP))
					continue;	// 파싱 불가한 ip 주소 방식

				line.replace(0, nColonPos, "");  // (:)콜론 이전의 글자 제거
				while ((int)(line.find(":")) >= 0)	// :(콜론) 제거
					line.replace(line.find(":"), 1, "");
				while ((int)(line.find(" ")) >= 0)
					line.replace(line.find(" "), 1, ""); // 공백 제거

				strcpy_s(arrEternetInfo[nCurCnt].IP, line.c_str());
				continue;
			}

			nPos = static_cast<int>(line.find(szKeyName_Subnet));
			if (nPos >= 0)
			{
				//영문 윈도 : Subnet Mask . . . . . . . . . . . : 255.255.0.0
				//한글 윈도 : 서브넷 마스크 . . . . . . . : 255.255.255.0
				//위의 문자열에서 주소를 파싱한다

				int nColonPos = static_cast<int>(line.find(":", strlen_i(szKeyName_Subnet)));
				if (nColonPos < strlen_i(szKeyName_Subnet))
					continue;	// 파싱 불가한 ip 주소 방식

				line.replace(0, nColonPos, "");  // (:)콜론 이전의 글자 제거
				while ((int)(line.find(":")) >= 0)	// :(콜론) 제거
					line.replace(line.find(":"), 1, "");
				while ((int)(line.find(" ")) >= 0)
					line.replace(line.find(" "), 1, ""); // 공백 제거

				strcpy_s(arrEternetInfo[nCurCnt].SubnetMask, line.c_str());
				continue;
			}

			nPos = static_cast<int>(line.find(szKeyName_Gateway));
			if (nPos >= 0)
			{
				//영문 윈도 : Default Gateway . . . . . . . . . : 192.168.0.1
				//한글 윈도 : 기본 게이트웨이 . . . . . . : 23.42.52.1
				//위의 문자열에서 주소를 파싱한다

				int nColonPos = static_cast<int>(line.find(":", strlen_i(szKeyName_Gateway)));
				if (nColonPos < strlen_i(szKeyName_Gateway))
					continue;	// 파싱 불가한 ip 주소 방식

				line.replace(0, nColonPos, "");  // (:)콜론 이전의 글자 제거
				while ((int)(line.find(":")) >= 0)	// :(콜론) 제거
					line.replace(line.find(":"), 1, "");
				while ((int)(line.find(" ")) >= 0)
					line.replace(line.find(" "), 1, ""); // 공백 제거

				strcpy_s(arrEternetInfo[nCurCnt].Gateway, line.c_str());
				continue;
			}

		}
		infile.close();
		DeleteFileA(tmpFilePath);    // 임시로 생성한 파일 삭제

		return nCurCnt + 1;
	}
	
	/**
	@ breif: 	Returns the thread count of the current process or -1 in case of failure.
	*/
	inline int GetCurrentThreadCount()
	{
		// first determine the id of the current process
		DWORD const  id = GetCurrentProcessId();

		// then get a process list snapshot.
		HANDLE const  snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);

		// initialize the process entry structure.
		PROCESSENTRY32 entry = { 0 };
		entry.dwSize = sizeof(entry);

		// get the first process info.
		BOOL  ret = true;
		ret = Process32First(snapshot, &entry);
		while (ret && entry.th32ProcessID != id) {
			ret = Process32Next(snapshot, &entry);
		}
		CloseHandle(snapshot);
		return ret
			? entry.cntThreads
			: -1;
	}


	inline bool GetDLLVersion(const char *filename, sFileViersionInfoEx *pVersion)
	{
		DWORD  verHandle = NULL;
		UINT   size = 0;
		LPBYTE lpBuffer = NULL;
		DWORD  verSize = GetFileVersionInfoSize(filename, &verHandle);
		//	bool bRet = true;

		if (verSize == 0)
			return false;

		LPSTR verData = new char[verSize];		//new

		if (GetFileVersionInfo(filename, verHandle, verSize, verData))
		{
			if (VerQueryValue(verData, "\\", (VOID FAR* FAR*)&lpBuffer, &size))
			{
				if (size)
				{
					VS_FIXEDFILEINFO *verInfo = (VS_FIXEDFILEINFO *)lpBuffer;
					if (verInfo->dwSignature == 0xfeef04bd)
					{
						pVersion->major = HIWORD(verInfo->dwFileVersionMS);
						pVersion->minor = LOWORD(verInfo->dwFileVersionMS);
						pVersion->build = HIWORD(verInfo->dwFileVersionLS);
						pVersion->branch = LOWORD(verInfo->dwFileVersionLS);
					}
				}
			}
		}
		delete[] verData;
		return true;
	}


	/**
	@breif  Find process running on OS by exe name
	@param  LPPROCESSENTRY32 out_processInfo : null ok. It menas that you just check the process is running.
	@return  returns true when find the process
	*/
	inline bool GetProcessInfoByName(std::string szExeName, LPPROCESSENTRY32 out_processInfo/* = nullptr*/)
	{
		HANDLE hndl = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		bool bFound = false;
		
		PROCESSENTRY32  procEntry = { 0 };
		procEntry.dwSize = sizeof(PROCESSENTRY32);
		if (out_processInfo != nullptr)
			procEntry = *out_processInfo;

		Process32First(hndl, &procEntry);
		do
		{
			if (!_strcmpi(procEntry.szExeFile, szExeName.c_str()))
			{
				bFound = true;
				break;
			}
		} while (Process32Next(hndl, &procEntry));

		if (out_processInfo != nullptr)
			*out_processInfo = procEntry;

		return bFound;
	}

	inline bool TerminateProcessByName(std::string szExeName, int retryCnt/* = 5*/, int retryInterval_ms/* = 100*/)
	{
		BOOL bFound = FALSE;
		DWORD dwExitCode = 0;

		PROCESSENTRY32  procEntry = { 0 };
		procEntry.dwSize = sizeof(PROCESSENTRY32);
		bFound = GetProcessInfoByName(szExeName, &procEntry);

		if (bFound == false)
			return true;

		int retry = 0;
		while (bFound)
		{
			HANDLE hHandle;
			hHandle = ::OpenProcess(PROCESS_ALL_ACCESS, 0, procEntry.th32ProcessID);

			::GetExitCodeProcess(hHandle, &dwExitCode);
			::TerminateProcess(hHandle, dwExitCode);

			bFound = GetProcessInfoByName(szExeName, &procEntry);
			if (bFound)
			{
				if (retry > retryCnt)
					break;
				Sleep(retryInterval_ms);
				retry++;
			}
		}
		return !bFound;
	}

	inline std::string GetLastErrorMessage(DWORD errorCode)
	{
		TCHAR* message = nullptr;
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
			nullptr,
			errorCode,
			0,// MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(TCHAR *)&message,
			0,
			nullptr);
		std::string retMsg;
		
		if (message)
		{
			retMsg += message;
			LocalFree(message);
		}
		return retMsg;
	}

	/**
	@author soohwan.cho
	@date 2020.05.07
	@brief Terminate current process forcely without "ZOMBIE" process status.
	*/
	inline void ForceExit()
	{
		DWORD dwExitCode;
		DWORD dwPID = GetCurrentProcessId();    // get my process ID

		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, dwPID);    // get process handle

		if (NULL != hProcess)
		{
			GetExitCodeProcess(hProcess, &dwExitCode);   // get exit code
			TerminateProcess(hProcess, dwExitCode);    // EXIT HERE!!!
			WaitForSingleObject(hProcess, INFINITE);
			CloseHandle(hProcess);
		}
	}
};