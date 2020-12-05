/**
* @author  soohwan.cho
* @date  2018.04.16
* @breif  All code was developed by soohwan.cho
*/

#pragma once

#ifndef _FN
#define _FN		__FUNCTION__
#endif
#ifndef _LN
#define _LN		__LINE__
#endif

#ifndef __IN__
#define __IN__	// function parameter IN argument
#endif
#ifndef __OUT__
#define __OUT__  // function parameter for [Call by Reference] return
#endif

#ifndef __MIN__
#define __MIN__(a, b)  (((a) < (b)) ? (a) : (b))
#endif

#ifndef __MAX__
#define __MAX__(a, b)  (((a) < (b)) ? (b) : (a))
#endif

//#define SET_IN_RANGE(val, minimum, maximum) val = __MIN__(val, maximum); val = __MAX__(val, minimum)
#define SET_IN_RANGE(val, minimum, maximum) (val < minimum ? minimum : (val > maximum ? maximum : val))
#define IsInRange(val, from, to)  ((val >= from) && (val <= to))
#define ToInt(val)				static_cast<int>(val)
#define ToDouble(val)		static_cast<double>(val)
#define CastTo(type, val)		static_cast<type>(val)
#define DyCastTo(type, val)		dynamic_cast<type>(val)
#define StrToChar(str) (str).c_str()
#define IsThisKeyExist_inMap(__yourMap, __key)		(__yourMap.find(__key) != __yourMap.end())
#define IsFileExists(szPath)  (GetFileAttributes(szPath) != INVALID_FILE_ATTRIBUTES && !(GetFileAttributes(szPath) & FILE_ATTRIBUTE_DIRECTORY))
#define WRITE_RAPID_LOG(logFileWithoutExt, isErrorLog, ...)  RapidLog2(logFileWithoutExt, isErrorLog, __FUNCTION__, __LINE__, __VA_ARGS__);
#define __CALL__(callFncName, func) callFncName = #func; func; 
#define DELETE_VECTOR_OBJECT(vector__) for(auto o : vector__) {delete o;}		vector__.clear()
#define DELETE_MAP_OBJECT(map__) for(auto o : map__) {delete o.second;};		map__.clear()
#define DELETE_OBJECT(obj) if(obj){delete obj; obj = nullptr;}
#define DELETE_ARRAY_OBJECT(obj) if(obj){delete []obj; obj = nullptr;}
#define IS_DIGIT(val)  (val >='0' && val <= '9')
