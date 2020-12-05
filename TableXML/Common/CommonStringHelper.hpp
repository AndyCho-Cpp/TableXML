/**
* @author  soohwan.cho
* @date  2018.04.16
* @breif  All code was developed by soohwan.cho
*/


#pragma once
#include "CommonDefine.hpp"
#include <string>
#include <stdarg.h>
#include <vector>


namespace CommonHelper
{
	// --------------------------------------------------------------------------------------------------------------------------------------
	// String Function
	//
	inline bool IsStringEqual(const char *a, const char *b, bool noCase = false);
	inline bool IsStringEqual(const std::string &a, const std::string &b, bool noCase = false);
	inline bool IsStringEqual(const char *a, const std::string &b, bool noCase = false);
	inline bool IsStringEqual(const std::string &a, const char *b, bool noCase = false);
	inline int strlen_i(const char *str);
	inline void ToUpperCase(char *str);
	inline void ToUpperCase(std::string &str);
	inline std::string UpperCaseString(std::string &str);
	inline bool IsEmptyString(const char *str);
	inline bool IsEmptyString(std::string str);
	inline std::string StringFormat(size_t size, const char* format, ...);
	inline std::string StringFormat1KB(const char* format, ...);
	inline bool IsDigit(const char *sz, int nSize, bool floatNum = false);
	inline bool TryParseDouble(const char *strTry, double *parsedValue);
	inline bool TryParseInt(const char *strTry, int *parsedValue, bool allowFloatNum = true);
	inline bool TryParseBool(const char *strTry, bool *parsedValue, bool allowFloatNum = true);
	inline int CountOfTokens(const char *sz, char tok);
	inline void ReplaceSubstring(std::string &str, std::string findStr, std::string toStr);
	inline std::vector<std::string> StrSplit(const std::string &str, char token, bool trimEmpty = false);
	inline void StrSplitEx(const std::string &str, char token, std::vector<std::string> &vout, bool trimEmpty = false);
	inline bool StrSplitSentensWord(const char *str, std::vector<std::string> *pOutArray, std::string *pErrMsg);
	inline std::string RelPathToAbsPath(std::string &relPath, bool endWithW = true);
	inline std::string CombinePath(std::string pathA, std::string pathB);
	inline std::string CombinePath(std::string pathA, std::string pathB, std::string pathC);
	inline std::string CombinePath(std::string pathA, std::string pathB, std::string pathC, std::string pathD);
	inline const char *last_strstr(const char *str, const char *findWord);
	inline std::string FilledString(int align_center0left1right2, std::string text, int width, char ch = ' ');
	inline void DecToComma(int n, std::string *pOut);
	inline std::string ExtractFileName(std::string fullPath, bool withoutExt = false);
	inline std::vector<int> ExtractNumber_inString(std::string strInput, char token);
	inline bool IsIpAddress(const char *szAddr);
	// --------------------------------------------------------------------------------------------------------------------------------------

	inline bool IsStringEqual(const char *a, const char *b, bool noCase/* = false*/)
	{
		if (noCase)
			return _strcmpi(a, b) == 0;

		return strcmp(a, b) == 0;
	}

	inline bool IsStringEqual(const std::string &a, const std::string &b, bool noCase/* = false*/) { return IsStringEqual(a.c_str(), b.c_str(), noCase); }
	inline bool IsStringEqual(const char *a, const std::string &b, bool noCase/* = false*/) { return IsStringEqual(a, b.c_str(), noCase); }
	inline bool IsStringEqual(const std::string &a, const char *b, bool noCase/* = false*/) { return IsStringEqual(a.c_str(), b, noCase); }
	inline int strlen_i(const char *str) { return		static_cast<int>(strlen(str)); }
	inline void ToUpperCase(char *str)
	{
		int k = 0;
		while (str[k] != '\0')
		{	str[k] = toupper(str[k]);
			k++;
		}
	}
	inline void ToUpperCase(std::string &str)
	{
		for (auto & c : str) 
			c = toupper(c);
	}

	inline std::string UpperCaseString(std::string &str)
	{
		std::string ret = str;
		for (auto & c : ret)
			c = toupper(c);
		return ret;
	}

	inline bool IsEmptyString(const char *str)
	{
		if (str == nullptr)
			return true;
		if (strlen(str) == 0)
			return true;
		return false;
	}
	inline bool IsEmptyString(std::string str)
	{
		return str.size() == 0;
	}

	inline std::string StringFormat(size_t size, const char* format, ...)
	{
		if (nullptr == format)
			return "";

		char *buf = new (std::nothrow) char[size];
		if (buf == NULL)
			return "";

		try
		{
			va_list args;

			va_start(args, format);
			vsnprintf_s(buf, size, size - 1, format, args);
			va_end(args);
		}
		catch (...) {}

		std::string str(buf);
		delete[]buf;
		return str;
	}

	inline std::string StringFormat1KB(const char* format, ...)
	{
		if (nullptr == format)
			return "";

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

		return std::string(buf);
	}


	/* @brief : see if it's digit or not
	* @author: soohwan.cho(@semes.com)
	* ex) input: "1234" return: true
	* ex) input: "123a4" return: false
	* ex) input: "123.4" return: false  dot(.) is not digit
	*/
	inline bool IsDigit(const char *sz, int nSize, bool floatNum/* = false*/)
	{
		int dotCount = 0;
		int minusCount = 0;
		if (nSize == 0 || sz == NULL)
			return false;

		for (int i = 0; i < nSize; i++)
		{
			if (sz[i] < '0' || sz[i] > '9')
			{
				if (floatNum && sz[i] == '.')
				{
					dotCount++;
					continue;
				}
				if (sz[i] == '-')
				{
					minusCount++;
					continue;
				}
				return false;
			}
		}
		if ((floatNum && dotCount > 1) || minusCount > 1)	return false;

		return true;
	}

	inline bool TryParseDouble(const char *strTry, double *parsedValue)
	{
		try
		{
			if (strTry == nullptr)
				return false;
			int len = static_cast<int>(strlen(strTry));
			if (len == 0)
				return false;

			std::string figure;

			const char space = ' ';
			const char dot = '.';
			const char nullChar = '\0';
			bool haveSign = false;
			bool haveDot = false;
			for (int k = 0; k < len; k++)
			{
				char prevChar = (k != 0) ? strTry[k - 1] : nullChar;
				char nextChar = (k - 1 != len) ? strTry[k + 1] : nullChar;
				char currChar = strTry[k];

				if (currChar == space)
				{
					if (figure.size() == 0)
						continue;
					if (nextChar == space || nextChar == nullChar)
						continue;
					return false;
				}
				else if (currChar == '-' || currChar == '+')
				{
					if (figure.size() != 0)
						return false;
					if (IS_DIGIT(nextChar) == false)
						return false;
					if (haveSign)
						return false;
					haveSign = true;
					figure += currChar;
				}
				else if (currChar == dot)
				{
					if (haveDot)
						return false;

					haveDot = true;
					figure += currChar;
				}
				else if (IS_DIGIT(currChar))
				{
					figure += currChar;
				}
				else
					return false;
			}

			if (figure.size() == 0)
				return false;
			bool haveDigit = false;
			for (auto c : figure)
			{
				if (IS_DIGIT(c))
				{
					haveDigit = true;
					break;
				}
			}
			if (haveDigit == false)
				return false;

			double dd = std::atof(figure.c_str());
			if (parsedValue)
				*parsedValue = dd;

		}
		catch (...)
		{
			return false;
		}
		return true;
	}

	inline bool TryParseInt(const char *strTry, int *parsedValue, bool allowFloatNum/* = true*/)
	{
		double dd = 0;
		if (TryParseDouble(strTry, &dd) == false)
			return false;

		if (dd > INT_MAX || dd < INT_MIN)
			return false;

		if (allowFloatNum == false)
		{
			if ((dd - static_cast<int>(dd)) != 0)
				return false;
		}
		if (parsedValue)
			*parsedValue = static_cast<int>(dd);
		return true;
	}


	inline bool TryParseBool(const char *strTry, bool *parsedValue, bool allowFloatNum/* = true*/)
	{
		try
		{
			if (strTry == nullptr)
				return false;
			int len = static_cast<int>(strlen(strTry));
			if (len == 0)
				return false;

			std::string figure;

			const char space = ' ';
			const char nullChar = '\0';
			for (int k = 0; k < len; k++)
			{
				char nextChar = (k - 1 != len) ? strTry[k + 1] : nullChar;
				char currChar = strTry[k];

				if (currChar == space)
				{
					if (figure.size() == 0)
						continue;
					else if (nextChar == space || nextChar == nullChar)
						continue;
					else
						return false;
				}
				figure += currChar;
			}

			if (_strcmpi(figure.c_str(), "TRUE") == 0 || _strcmpi(figure.c_str(), "1") == 0)
			{
				if (parsedValue)
					*parsedValue = true;
			}
			else if (_strcmpi(figure.c_str(), "FALSE") == 0 || _strcmpi(figure.c_str(), "0") == 0)
			{
				if (parsedValue)
					*parsedValue = false;
			}
			else
				return false;

		}
		catch (...) { return false; }
		return true;
	}

	/* @brief : CountOfTokens
	* @author: soohwan.cho(@semes.com)
	* ex) input: "255.255.255.255", ','  return:  3
	* ex) input: "123a4" return: false
	* ex) input: "123.4" return: false  dot(.) is not digit
	*/
	inline int CountOfTokens(const char *sz, char tok)
	{
		if (sz == NULL)
			return 0;

		int count = 0;
		size_t nSize = strlen(sz);

		for (size_t i = 0; i < nSize; i++)
		{
			if (sz[i] == tok)
				count++;
		}

		return count;
	}

	inline void ReplaceSubstring(std::string &str, std::string findStr, std::string toStr)
	{
		size_t index = 0;
		while ((index = str.find(findStr, index)) != std::string::npos)
		{
			str.replace(index, findStr.size(), toStr);
			index += toStr.size();
		}
	}
	/* @brief : string split and returns to vector
	* @author: soohwan.cho(@semes.com)
	*/
	inline std::vector<std::string> StrSplit(const std::string &str, char token, bool trimEmpty/* = false*/)
	{
		std::vector<std::string> result;
		int size = static_cast<int>(str.size());
		int start = 0;
		int end = 0;
		do
		{
			int pos = static_cast<int>(str.find(token, start));
			end = pos == -1 ? size : pos;
			if (trimEmpty ? (start < end) : (start <= end))
				result.push_back(str.substr(start, end - start));
			start = end + 1;

		} while (end < size);

		return result;
	}

	/**
	* @brief : string split and returns to vector
	* @author: soohwan.cho(@semes.com)
	* @ps: keep author define comment above
	*/
	inline void StrSplitEx(const std::string &str, char token, std::vector<std::string> &vout, bool trimEmpty/* = false*/)
	{
		int size = static_cast<int>(str.size());
		int start = 0;
		int end = 0;
		do
		{
			int pos = static_cast<int>(str.find(token, start));
			end = pos == -1 ? size : pos;
			if (trimEmpty ? (start < end) : (start <= end))
				vout.push_back(str.substr(start, end - start));
			start = end + 1;

		} while (end < size);
	}


	/**
	* @brief  String spliting every word and sentens.
	  @code  File Copy "C:\My Code .h" "D:\ Target forder"
	  @result   Split every word and make it wrap between "..."
	  @author  soohwan.cho
	  @date  2020.04.27
	*/
	inline bool StrSplitSentensWord(const char *str, std::vector<std::string> *pOutArray, std::string *pErrMsg)
	{
		if (str == nullptr)
		{
			if (pErrMsg)
				*pErrMsg = "Null string.";
		}
		int len = static_cast<int>(strlen(str));
		if (len == 0)
		{
			if (pErrMsg)
				*pErrMsg = "Empty String";
		}
		if (pOutArray == nullptr)
		{
			if (pErrMsg)
				*pErrMsg = "Null output array pointer.";
		}

		std::string word;

		const char space = ' ';
		const char tab = '\t';
		const char lineFeed = '\n';
		const char quotes = '\"';
		bool quotesOpen = false;
		const char nullChar = '\0';
		char prevChar = nullChar;
		char currChar = nullChar;
		char nextChar = nullChar;
		bool flushed = true;

		for (int k = 0; k < len; k++)
		{
			currChar = str[k];
			nextChar = (k != len - 1) ? str[k + 1] : nullChar;
			prevChar = (k != 0) ? str[k - 1] : nullChar;

			if (currChar == space || currChar == tab || currChar == lineFeed)
			{
				if (word.size() == 0)	// skip white space
					continue;

				if (quotesOpen)
				{
					word += currChar;	// quotes open. Literally append space char.
					flushed = false;
				}
				else
				{
					pOutArray->push_back(word);	// end of a word.
					word.clear();	//reset word
					flushed = true;
				}
			}
			else if (currChar == quotes)
			{
				if (quotesOpen)
				{
					if (nextChar == nullChar || nextChar == space || nextChar == tab || nextChar == lineFeed)
					{
						quotesOpen = false; // quotes close!
						pOutArray->push_back(word);	// end of a sentens.
						word.clear();	//reset word
						flushed = true;
					}
					else
					{
						word += currChar;	// Even it's a quotes char, it's not related to quotes. It's literal.
						flushed = false;
					}
				}
				else
				{
					if (prevChar == nullChar || prevChar == space || prevChar == space || prevChar == tab || prevChar == lineFeed)
					{
						quotesOpen = true;
					}
					else
					{
						word += currChar;	// Even it's a quotes char, it's not related to quotes. It's literal.
						flushed = false;
					}
				}
			}
			else
			{
				word += currChar;
				flushed = false;
			}

		}

		//Error case. String ended without quotes close.
		if (quotesOpen)
		{
			quotesOpen = false; // quotes close!
			pOutArray->push_back(word);	// end of a sentens.
			word.clear();	//reset word

			if (pErrMsg)
				*pErrMsg = "String ended without quotes close.";
			return false;
		}

		if (flushed == false && word.size() != 0)
		{
			pOutArray->push_back(word);
			flushed = false;
		}

		return true;
	}
	/**
	*  @breif: path change. relative path to absolute path.
	*  @author : soohwan.cho(@semes.com)
	*  @date : 2019.10.13

	* @example
		Relative Path : C:\DirA\DirB\DirC\DirD\DirE\..\..\..\bin\App\..\..\Src
		return Path : C:\DirA\DirB\Src\

		@ endWithW : end with \ at the last of path
	*/
	inline std::string RelPathToAbsPath(std::string &relPath, bool endWithW/* = true*/)
	{
		std::vector<std::string> vlist = StrSplit(relPath, '\\', true);
		std::vector<std::string> absList;

		for (size_t k = 0; k < vlist.size(); k++)
		{
			if (vlist[k] == "..")
			{
				if (absList.size() == 0)
					return "";	 //exception case
				absList.erase(absList.end() - 1);
				continue;
			}
			else if (vlist[k] == ".")
				continue;

			absList.push_back(vlist[k]);
		}

		if (absList.size() == 0)
			return "";

		std::string absPath;
		for (auto o : absList)
			absPath += o + "\\";

		if (endWithW == false)	// remove last \ sign
			absPath.erase(absPath.end() - 1);

		return absPath;
	}

	/**
	  @breif: combine two path. This arranges two path. Also chagne to absolute path.
	  @author : soohwan.cho(@semes.com)
	  @date : 2020.01.26

	 @example
		pathA : C:\DirA
		pathB : file.txt
		 => return : C:\DirA\file.txt

		pathA : C:\DirA\Bin\..\..\
		pathB : Data/file.txt
		 => return : C:\Data\file.txt
		 @refer: https://docs.microsoft.com/ko-kr/dotnet/api/microsoft.visualbasic.fileio.filesystem.combinepath?view=netframework-4.8
	*/
	inline std::string CombinePath(std::string pathA, std::string pathB)
	{
		// slash to back slush
		ReplaceSubstring(pathA, "/", "\\");
		ReplaceSubstring(pathB, "/", "\\");

		int lenA = static_cast<int>(pathA.length());
		int lenB = static_cast<int>(pathB.length());
		bool pathAEndWithDir = false;
		bool pathBBeginWithDir = false;
		if (lenA > 0)
			pathAEndWithDir = pathA.at(lenA - 1) == '\\' || pathA.at(lenA - 1) == '/';
		if (lenB > 0)
			pathBBeginWithDir = pathB.at(0) == '\\' || pathB.at(0) == '/';

		if (pathAEndWithDir == false && pathBBeginWithDir == false)
			pathA = pathA + "\\";
		else if (pathAEndWithDir && pathBBeginWithDir)
			pathA.erase(pathA.end());

		std::string combinePath = pathA + pathB;
		return RelPathToAbsPath(combinePath, false);
	}
	//@refer: https://docs.microsoft.com/ko-kr/dotnet/api/microsoft.visualbasic.fileio.filesystem.combinepath?view=netframework-4.8
	inline std::string CombinePath(std::string pathA, std::string pathB, std::string pathC)
	{
		return CombinePath(CombinePath(pathA, pathB), pathC);
	}
	//@refer: https://docs.microsoft.com/ko-kr/dotnet/api/microsoft.visualbasic.fileio.filesystem.combinepath?view=netframework-4.8
	inline std::string CombinePath(std::string pathA, std::string pathB, std::string pathC, std::string pathD)
	{
		return CombinePath(CombinePath(CombinePath(pathA, pathB), pathC), pathD);
	}

	/* @brief : IPv4 Address string validation check. brute-force technique ;;; soon or later it will change to std::regex + smatch
	* @author: soohwan.cho(@semes.com)
	*/
	inline bool IsIpAddress(const char *szAddr)
	{
		try
		{
			if (szAddr == NULL)
				return false;

			size_t nSize = strlen(szAddr);

			if (nSize < 7 || nSize > 15)	//0.0.0.0 ~ 255.255.255.255
				return false;

			std::vector<std::string> arr = StrSplit(szAddr, '.', true);

			int nDotCnt = CountOfTokens(szAddr, '.');

			int nCnt = static_cast<int>(arr.size());

			if (nCnt != 4)
				return false;

			if (strcmp("0.0.0.0", szAddr) == 0)
				return false;

			for (int i = 0; i < nCnt; i++)
			{
				char *pStr = (char*)arr[i].c_str();
				int nLen = static_cast<int>(arr[i].length());

				if (IsDigit(pStr, nLen) == false)
					return false;

				int nVal = atoi(pStr);
				if (nVal < 0 || nVal > 255)
					return false;
			}
		}
		catch (...)
		{
			return false;
		}

		return true;
	}


	/* @brief : extrant number in string
	* in: "1,4,6,7,10", ','   return : vector<int> {1, 4, 6, 7, 10}
	* @author: soohwan.cho(@semes.com)
	*/
	inline std::vector<int> ExtractNumber_inString(std::string strInput, char token)
	{
		std::vector<int> arrInt;

		std::vector<std::string> arrStr = StrSplit((char*)strInput.c_str(), token, true);
		int nArrSize = static_cast<int>(arrStr.size());

		for (int i = 0; i < nArrSize; i++)
		{
			char *pStr = (char*)arrStr[i].c_str();
			int nLen = static_cast<int>(arrStr[i].length());

			if (IsDigit(pStr, nLen) == false)
				continue;

			int nVal = atoi(pStr);
			arrInt.push_back(nVal);
		}

		return arrInt;
	}
	inline std::string ExtractFileName(std::string fullPath, bool withoutExt/* = false*/)
	{
		char drive[_MAX_DRIVE];
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];
		char ext[_MAX_EXT];

		_splitpath_s(fullPath.c_str(), drive, dir, fname, ext);
		if (withoutExt)
			return std::string(fname);
		else
			return std::string(fname) + ext;
	}

	/**
 @author : soohwan.cho
 @date: 2019.11.14
 @function: DecToComma
 @brief: Add comma every thounds position 10000000 -> 10,000,000
*/
	inline void DecToComma(int n, std::string *pOut)
	{
		if (n < 0)
		{
			*pOut += "-";
			DecToComma(-n, pOut);
			return;
		}
		if (n < 1000)
		{
			*pOut += std::to_string(n);
			return;
		}
		DecToComma(n / 1000, pOut);

		char tmp[20];
		sprintf_s(tmp, ",%03d", n % 1000);
		*pOut += tmp;
	}

	/**
		@function    FilledString
		@brief   string filling function
		@parameter    align_center0left1right2 : 0 means center align,  1 means left align,  2 means right align
		@example   FilledString(0,  "soohwan", 11, '*')  --> output:  **soohwan**
		@author  soohwan.cho
		@date    2020.02.20
	*/
	inline std::string FilledString(int align_center0left1right2, std::string text, int width, char ch/* = ' '*/)
	{
		int len = static_cast<int>(text.length());
		int space = width - len;
		if (space < 1)
			return text;

		//center case
		if (align_center0left1right2 == 0)
		{
			int leftSpace = space / 2;
			int rightSpace = space - leftSpace;
			return std::string(leftSpace, ch) + text + std::string(rightSpace, ch);
		}
		// left case
		else if (align_center0left1right2 == 1)
		{
			return text + std::string(space, ch);
		}
		else
		{
			return std::string(space, ch) + text;
		}
	}
	   	
	// Reversed strstr
	inline const char *last_strstr(const char *str, const char *findWord)
	{
		if (*findWord == '\0')
			return (char *)str;

		const char *result = NULL;
		for (;;) {
			const char *p = strstr(str, findWord);
			if (p == NULL)
				break;
			result = p;
			str = p + 1;
		}

		return result;
	}
};