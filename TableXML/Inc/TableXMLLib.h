#pragma once
#ifdef TABLEXMLLIB_EXPORTS  
#define TBLXML_API __declspec(dllexport)
#else
#define TBLXML_API __declspec(dllimport) 	

#ifndef _DEBUG
	#pragma comment (lib, "TableXMLLib.lib")
#else
	#pragma comment (lib, "TableXMLLib_d.lib")
#endif
#endif
