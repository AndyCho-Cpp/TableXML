/**
* @author  soohwan.cho
* @date  2018.04.16
* @breif  All code was developed by soohwan.cho
*/


#pragma once
#include "CommonDefine.hpp"
#include "CommonStringHelper.hpp"
#include <Windows.h>
#include <string>

#pragma warning(disable : 4267)
/**
* @author: soohwan.cho(@semes.com)
* @brief: sendHeader file only. common help function, class, define, etc...
* @date : 2018.04.16
* @ps: if you think this is a lots of helps on your project... call donate()
*/
namespace CommonHelper
{
	// --------------------------------------------------------------------------------------------------------------------------------------
	// Utility Function
	//
	inline void SET_BIT(void* pMem, unsigned int nBit, bool bSet);
	inline bool GET_BIT(void* pMem, unsigned int nBit);
	inline bool CreateDirectoryDeeply(const char* dirName);
	inline std::string GetRunDirPath();
	class MinMaxAvr;
	class ValueCompLogic;
	// ---------------------------------------------------------------------------------------------------------------------------------------


	inline void SET_BIT(void* pMem, unsigned int nBit, bool bSet)
	{
		unsigned char* pByte = (unsigned char*)pMem + (unsigned int)(nBit / 8);
		nBit = nBit % 8;

		if (bSet)
		{
			*pByte |= (1 << nBit);
		}
		else
		{
			*pByte &= ~(1 << nBit);
		}
	}
	inline bool GET_BIT(void* pMem, unsigned int nBit)
	{
		unsigned char* pByte = (unsigned char*)pMem + (unsigned int)(nBit / 8);
		nBit = nBit % 8;

		return (*pByte >> nBit) & 1;
	}
	inline bool CreateDirectoryDeeply(const char* dirName)
	{
		if (nullptr == dirName)
			return false;

		char Buff;
		char szBuff[MAX_PATH];
		strcpy_s(szBuff, dirName);
		char* cpCurr = szBuff;

		for (int i = 0; i < (int)strlen(cpCurr); i++)
		{
			switch (*(cpCurr + i))
			{
			case '/':
			case '\\':
				Buff = *(cpCurr + i);
				*(cpCurr + i) = NULL;
				if (strlen(szBuff) > 0 && *(szBuff + strlen(szBuff) - 1) != ':')
				{
					if (CreateDirectoryA(szBuff, 0) == FALSE)
					{
						auto LastError = GetLastError();
						if (LastError != ERROR_ALREADY_EXISTS)
						{
							return false;
						}
					}
				}

				*(cpCurr + i) = Buff;
				break;
			}
		}

		if (CreateDirectoryA(dirName, 0) == FALSE)
		{
			auto LastError = GetLastError();
			if (LastError != ERROR_ALREADY_EXISTS)
			{
				return false;
			}
		}

		return true;
	}
	inline std::string GetRunDirPath()
	{
		// get module path
		char modulePath[_MAX_PATH];
		GetModuleFileNameA(NULL, modulePath, _MAX_PATH);

		char path_buffer[_MAX_PATH];
		char drive[_MAX_DRIVE];
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];
		char ext[_MAX_EXT];
		char buf[_MAX_PATH];

		strcpy_s(path_buffer, _MAX_PATH, modulePath);
		//wcstombs(path_buffer, modulePath, _MAX_PATH);
		_splitpath_s(path_buffer, drive, dir, fname, ext);

		memset(buf, '\0', sizeof(buf));
		strcat_s(buf, drive);
		strcat_s(buf, dir);
		return std::string(buf);
	}


	class MinMaxAvr
	{
		int min, max, addCnt;
		int sum;
		int rangeFrom, rangeTo;
		
	public:
		const static unsigned int ADD_LIMIT_COUNT = 10000;

		MinMaxAvr() { Init(); }
		void Init()
		{
			rangeFrom = INT_MIN;
			rangeTo = INT_MAX;
			min = INT_MAX;
			max = INT_MIN;
			addCnt = 0;
			sum = 0;
		}
		void RangeOfInterest(int from, int to)
		{
			rangeFrom = from;
			rangeTo = to;
		}
		void AddVal(int val)
		{
			if (IsInRange(val, rangeFrom, rangeTo) == false)
				return;

			if (addCnt == ADD_LIMIT_COUNT)
			{
				sum = static_cast<int>(GetAverage());
				addCnt = 1;
			}

			if (val < min)
				min = val;
			if (val > max)
				max = val;
			sum += val;
			addCnt++;
		}
		int GetMin() { return min == INT_MAX ? 0 : min; }
		int GetMax() { return max == INT_MIN ? 0 : max; }
		int GetCount() { return addCnt; }
		int GetSum() { return sum; }
		double GetAverage()
		{
			if (addCnt == 0)
				return 0;
			return (double)sum / addCnt;
		}
	};

	class ValueCompLogic
	{
		enum class eCompSign : char { less, less_equal, equal, greater_equal, greater, _not, contain };

		std::string itemName;
		eCompSign compSign;
		std::string value;
		bool digitVal;
		double parsedVal;

	public:
		ValueCompLogic()
		{
			itemName = value = "";
			compSign = eCompSign::equal;
			digitVal = false;
			parsedVal = 0;
		}
		bool Set(std::string _name, std::string _compSign, std::string _value)
		{
			digitVal = TryParseDouble(_value.c_str(), &parsedVal);
			value = _value;
			itemName = _name;

			if (_compSign == "<")
				compSign = eCompSign::less;
			else if (_compSign == "<=" || _compSign == "=<")
				compSign = eCompSign::less_equal;
			else if (_compSign == "=" || _compSign == "==")
				compSign = eCompSign::equal;
			else if (_compSign == ">=" || _compSign == "=>")
				compSign = eCompSign::greater_equal;
			else if (_compSign == ">")
				compSign = eCompSign::greater;
			else if (_compSign == "!" || _compSign == "!=" || _compSign == "=!")
				compSign = eCompSign::_not;
			else if (_compSign == "@")
				compSign = eCompSign::contain;
			else
				return false;
			return true;
		}
		bool DoCompare(std::string &compVal)
		{
			if (digitVal)
			{
				double parseCompVal = 0;
				if (TryParseDouble(compVal.c_str(), &parseCompVal) == false)
					return false;

				switch (compSign)
				{
				case eCompSign::less:					return parsedVal < parseCompVal;
				case eCompSign::less_equal:		return parsedVal <= parseCompVal;
				case eCompSign::equal:				return parsedVal == parseCompVal;
				case eCompSign::greater_equal:	return parsedVal >= parseCompVal;
				case eCompSign::greater:			return parsedVal > parseCompVal;
				case eCompSign::_not:				return parsedVal != parseCompVal;
				default:									return false;
				}
			}
			else
			{
				switch (compSign)
				{
				case eCompSign::equal: return IsStringEqual(value, compVal);
				case eCompSign::_not:	return !IsStringEqual(value, compVal);
				case eCompSign::contain: return UpperCaseString(value).find(UpperCaseString(compVal)) != std::string::npos;
				default:									return false;
				}
			}
			return false;
		}
	};

	

};;