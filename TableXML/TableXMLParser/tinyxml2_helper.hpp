#pragma once
#include "tinyxml2.h"
#include <unordered_map>
#include <map>
#include <Windows.h>
//#include "CommonFunction.hpp"
#include <comdef.h>
#include "tinyxml2_helper_attribute_node.hpp"

//using namespace CommonHelper;
using namespace tinyxml2;

#pragma warning(disable : 4267)


inline const char *TinyErrorToString(XMLError errCode)
{
	switch(errCode)
	{	
	case XMLError::XML_SUCCESS:	return "XML_SUCCESS";
	case XMLError::XML_NO_ATTRIBUTE: return "XML_NO_ATTRIBUTE";
	case XMLError::XML_WRONG_ATTRIBUTE_TYPE: return "XML_WRONG_ATTRIBUTE_TYPE";
	case XMLError::XML_ERROR_FILE_NOT_FOUND: return "XML_ERROR_FILE_NOT_FOUND";
	case XMLError::XML_ERROR_FILE_COULD_NOT_BE_OPENED: return "XML_ERROR_FILE_COULD_NOT_BE_OPENED";
	case XMLError::XML_ERROR_FILE_READ_ERROR: return "XML_ERROR_FILE_READ_ERROR";
	case XMLError::XML_ERROR_PARSING_ELEMENT: return "XML_ERROR_PARSING_ELEMENT";
	case XMLError::XML_ERROR_PARSING_ATTRIBUTE: return "XML_ERROR_PARSING_ATTRIBUTE";
	case XMLError::XML_ERROR_PARSING_TEXT: return "XML_ERROR_PARSING_TEXT";
	case XMLError::XML_ERROR_PARSING_CDATA: return "XML_ERROR_PARSING_CDATA";
	case XMLError::XML_ERROR_PARSING_COMMENT: return "XML_ERROR_PARSING_COMMENT";
	case XMLError::XML_ERROR_PARSING_DECLARATION: return "XML_ERROR_PARSING_DECLARATION";
	case XMLError::XML_ERROR_PARSING_UNKNOWN: return "XML_ERROR_PARSING_UNKNOWN";
	case XMLError::XML_ERROR_EMPTY_DOCUMENT: return "XML_ERROR_EMPTY_DOCUMENT";
	case XMLError::XML_ERROR_MISMATCHED_ELEMENT: return "XML_ERROR_MISMATCHED_ELEMENT";
	case XMLError::XML_ERROR_PARSING: return "XML_ERROR_PARSING";
	case XMLError::XML_CAN_NOT_CONVERT_TEXT: return "XML_CAN_NOT_CONVERT_TEXT";
	case XMLError::XML_NO_TEXT_NODE: return "XML_NO_TEXT_NODE";
	case XMLError::XML_ELEMENT_DEPTH_EXCEEDED: return "XML_ELEMENT_DEPTH_EXCEEDED";
	case XMLError::XML_ERROR_COUNT: return "XML_ERROR_COUNT";
	};
	return "";
}

//
//char * __fastcall Utf8ToAscii(std::string& AUtf8)
////==============================================================================
//{
//	wchar_t *wsz;
//	char *sAscii;
//
//	int wLen = MultiByteToWideChar(CP_UTF8, 0, AUtf8.c_str(), AUtf8.length(), NULL, 0);
//	wsz = new wchar_t[wLen];
//	
//	MultiByteToWideChar(CP_UTF8, 0, AUtf8.c_str(), AUtf8.length(), wsz, wLen);
//
//	int mbLen = WideCharToMultiByte(CP_ACP, 0, wsz, wLen, NULL, 0, NULL, NULL);
//
//	sAscii = new char[mbLen];
//	WideCharToMultiByte(CP_ACP, 0, wsz, wLen, sAscii, mbLen, NULL, NULL);
//
//	return sAscii.c_str();
//}

//void Tour(XMLNode* parent)
//{
//	if (!parent)
//		return;
//
//	for (XMLNode* child = (XMLNode*)parent->FirstChildElement() ; child != nullptr ; child = child->NextSiblingElement())
//	{
//		XMLElement* elem = (XMLElement*)child;
//		const char* str = elem->GetText();
//		if (str)
//		{
//			printf("%s = %s\n", elem->Name(), str);
//		}
//		wchar_t wsz[4096] = { 0 };
//		char sAscii[4096] = { 0 };
//
//		for (const XMLAttribute* attr = elem->FirstAttribute(); attr != nullptr ; attr = attr->Next())
//		{	
//			memset(wsz, 0, sizeof(wsz));
//			memset(sAscii, 0, sizeof(sAscii));
//
//			// Get Original UTF8 string length
//			std::string utf8Org = attr->Value();
//			int wLen = MultiByteToWideChar(CP_UTF8, 0, utf8Org.c_str(), utf8Org.size(), NULL, 0);
//			
//			// UTF8 -> Wide Char String
//			MultiByteToWideChar(CP_UTF8, 0, utf8Org.c_str(), utf8Org.size(), wsz, wLen);
//			
//			// Get Wide Char String Length
//			int mbLen = WideCharToMultiByte(CP_ACP, 0, wsz, wLen, NULL, 0, NULL, NULL);
//			
//			// Wide Char String -> Muilbyte Char String
//			WideCharToMultiByte(CP_ACP, 0, wsz, wLen, sAscii, mbLen, NULL, NULL);
//
//			printf("%s [%s] = %s\n", elem->Name(), attr->Name(), sAscii);
//
//		}
//		Tour(elem);
//	}
//}

inline int GetAllAttributes_inOneNode(XMLNode *node, std::unordered_map<std::string/*attr name*/, std::string/*value*/> *outList)
{
	XMLElement* elem = (XMLElement*)node;
	wchar_t wsz[4096] = { 0 };
	char sAscii[4096] = { 0 };

	for (const XMLAttribute* attr = elem->FirstAttribute(); attr != nullptr; attr = attr->Next())
	{
		memset(wsz, 0, sizeof(wsz));
		memset(sAscii, 0, sizeof(sAscii));

		// Get Original UTF8 string length
		std::string utf8Org = attr->Value();
		int wLen = MultiByteToWideChar(CP_UTF8, 0, utf8Org.c_str(), utf8Org.size(), NULL, 0);

		// UTF8 -> Wide Char String
		MultiByteToWideChar(CP_UTF8, 0, utf8Org.c_str(), utf8Org.size(), wsz, wLen);

		// Get Wide Char String Length
		int mbLen = WideCharToMultiByte(CP_ACP, 0, wsz, wLen, NULL, 0, NULL, NULL);

		// Wide Char String -> Muilbyte Char String
		WideCharToMultiByte(CP_ACP, 0, wsz, wLen, sAscii, mbLen, NULL, NULL);

		(*outList)[attr->Name()] = sAscii;
		//printf("%s [%s] = %s\n", elem->Name(), attr->Name(), sAscii);
	}
	return outList->size();
}

using AttributePair = std::pair<std::string, sAttributeNode>;
using AttributeHash = std::map<std::string, sAttributeNode>;

inline int GetAllAttributes_inOneNode(XMLNode *node, AttributeHash *outList)
{
	XMLElement* elem = (XMLElement*)node;
	wchar_t wsz[4096] = { 0 };
	char sAscii[4096] = { 0 };

	for (const XMLAttribute* attr = elem->FirstAttribute(); attr != nullptr; attr = attr->Next())
	{
		memset(wsz, 0, sizeof(wsz));
		memset(sAscii, 0, sizeof(sAscii));

		// Get Original UTF8 string length
		std::string utf8Org = attr->Value();
		int wLen = MultiByteToWideChar(CP_UTF8, 0, utf8Org.c_str(), utf8Org.size(), NULL, 0);

		// UTF8 -> Wide Char String
		MultiByteToWideChar(CP_UTF8, 0, utf8Org.c_str(), utf8Org.size(), wsz, wLen);

		// Get Wide Char String Length
		int mbLen = WideCharToMultiByte(CP_ACP, 0, wsz, wLen, NULL, 0, NULL, NULL);

		// Wide Char String -> Muilbyte Char String
		WideCharToMultiByte(CP_ACP, 0, wsz, wLen, sAscii, mbLen, NULL, NULL);
		
		// insert into hash
		outList->insert(AttributePair(attr->Name(), sAttributeNode(attr->Name(), sAscii, attr)));
		//printf("%s [%s] = %s\n", elem->Name(), attr->Name(), sAscii);
	}
	return outList->size();
}


//void test(const char *xmlPath)
//{	//
//	//tinyxml2::XMLDocument doc;
//	//tinyxml2::XMLError error = tinyxml2::XMLError::XML_SUCCESS;
//	//const int retryNum = 5;
//	
//	//TimePoint tp = StdClock::Now_t();
//
//	//do
//	//{
//	//	error = doc.LoadFile(xmlPath);
//	//	if (tinyxml2::XMLError::XML_SUCCESS == error)
//	//		break;
//	//
//	//	printf("error code: %d\n", error);
//	//	Sleep(100);
//	//
//	//} while (StdClock::Elapsed_ms(tp) < 3000);
//
//	//Tour(doc.FirstChildElement());
//}