#pragma once
#include "IOSDataType.h"
#include "CommonFunction.hpp"
#include "IOAppInterfaceApi.h"
#include "logger.h"
#define SMA_NAME_IO_IMAGE_DRV_IO			"IOS.IMAGE.IOLIST_BY_DRV"
//#define SMA_NAME_IO_IMAGE_MODULE_IO	"IOS.IMAGE.IOLIST_BY_MODULE"
#define SMA_NAME_IO_IMAGE_ENUM			"IOS.IMAGE.ENUM_LIST"
#define SMA_NAME_REMOTE_IO_IMAGE			"IOS.IMAGE.REMOTE_IO_LIST"

#define SMA_REQ_WRITE_BUFFER_MAX_COUNT			1000
//#define SMA_REP_WRITE_BUFFER_MAX_COUNT			100


#define IO_APP_WRITE "AW"
#define IO_DRIVER_WRITE "DW"
enum class eChangedIOCmd : char
{
	eChangedIO = 10
};

// SMA Name : Driver IO Value Read Table
inline std::string GetSmaName_forDriverReadTable(std::string driverName)
{
	std::string tmp = CommonHelper::StringFormat1KB("IOS.%s.SMA_VALUE", driverName.c_str());
	return tmp;
}

// SMA Name: Request Write
inline std::string GetSmaName_forREQWrite(std::string driverName)
{
	std::string tmp = CommonHelper::StringFormat1KB("IOS.%s.SMA_WREQ", driverName.c_str());
	return tmp;
}

// SMA Name : Response Write
//inline std::string GetSmaName_forREPWrite(std::string driverName)
//{
//	std::string tmp = CommonHelper::StringFormat1KB("IOS.%s.SMA_WREP", driverName.c_str());
//	return tmp;
//}

// SMA name : Driver Time Stamp Name
inline std::string GetSmaName_forDriverTimeStamp(std::string driverName)
{
	std::string tmp = CommonHelper::StringFormat1KB("IOS.%s.TIME_STAMP", driverName.c_str());
	return tmp;
}

// Mutex Name : Request Write
inline std::string GetMutexName_forREQWrite(std::string driverName)
{
	std::string tmp = CommonHelper::StringFormat1KB("IOS.%s.MTX_WREQ", driverName.c_str());
	return tmp;
}

// Mutex Name: Response Write
//inline std::string GetMutexName_forREPWrite(std::string driverName)
//{
//	std::string tmp = CommonHelper::StringFormat1KB("IOS.%s.MTX_WREP", driverName.c_str());
//	return tmp;
//}

// Mutex Name: Read Table
inline std::string GetMutexName_forRead(std::string driverName)
{
	std::string tmp = CommonHelper::StringFormat1KB("IOS.%s.MTX_READ", driverName.c_str());
	return tmp;
}

// Event name : Request write
inline std::string GetEventName_forREQWrite(std::string driverName)
{
	std::string tmp = CommonHelper::StringFormat1KB("IOS.%s.EVT_WREQ", driverName.c_str());
	return tmp;
}

inline std::string EnumAttrToString(sEnumInfo_t *info)
{
	std::string prefix;
	std::string msg = std::string(info->name) + "\n";

	for (int k = 0; k < info->count; k++)
	{
		msg += CommonHelper::StringFormat1KB("\t[%s] : [%d]\n", +info->items[k].name, info->items[k].val);
	}
	return msg;
}

inline std::string DriverAttrToString(sIODriverInfo *info)
{

	std::string msg;
	msg += CommonHelper::StringFormat1KB("FullName: %s\n", info->fullName);
	msg += CommonHelper::StringFormat1KB("Name: %s\n", info->name);
	msg += CommonHelper::StringFormat1KB("controllerName: %s\n", info->controllerName);
	msg += CommonHelper::StringFormat1KB("moduleName: %s\n", info->moduleName);
	msg += CommonHelper::StringFormat1KB("needLoad: %s\n", info->needLoad ? "true" : "false");
	msg += "arg1: " + std::to_string(info->args[0]) + "\n";
	msg += "arg2: " + std::to_string(info->args[1]) + "\n";
	msg += "arg3: " + std::to_string(info->args[2]) + "\n";
	msg += "arg4: " + std::to_string(info->args[3]) + "\n";
	msg += "arg5: " + std::to_string(info->args[4]) + "\n";
	msg += "arg6: " + std::to_string(info->args[5]) + "\n";
	msg += "arg7: " + std::to_string(info->args[6]) + "\n";
	msg += "arg8: " + std::to_string(info->args[7]) + "\n";
	msg += "arg9: " + std::to_string(info->args[8]) + "\n";
	msg += "arg10: " + std::to_string(info->args[9]) + "\n";

	msg += CommonHelper::StringFormat1KB("strArg: %s\n", info->strArg);
	msg += CommonHelper::StringFormat1KB("stringKeyType: %s\n", info->stringKeyType ? "true" : "false");
	msg += CommonHelper::StringFormat1KB("alwaysAlive: %s\n", info->alwaysAlive ? "true" : "false");
	msg += CommonHelper::StringFormat1KB("publicDriver: %s\n", info->publicDriver ? "true" : "false");

	return msg;
}

// Event name : Response write
//inline std::string GetEventName_forREPWrite(std::string driverName)
//{
//	std::string tmp = CommonHelper::StringFormat1KB("IOS.%s.EVT_WREP", driverName.c_str());
//	return tmp;
//}

#pragma pack( push, 1 )
/**
* @brief: shared memory buffer header
*/
struct sSharedBufferInfo
{
	int count;
	int pos;
	int size;
};
#pragma pack( pop )

inline std::string VarIOToString(sVarIO val, const char *ioFullName)
{
	std::string ret;
	switch (val.type)
	{
	case eIOType::Enum:
	{
		//sEnumInfo_t enumInfo;
		//ret = "Undefined Enum";
		//if (IOS_GetEnumInfo(enumName.c_str(), &enumInfo) == false)
		//	break;

		char sz[64];
		memset(sz, NULL, sizeof(sz));
		if (IOS_EnumValueToString(ioFullName, val.value._enum, sz, sizeof(sz)) == false)
			strcpy_s(sz, "[Enum Error]");
		ret = sz;
		//for (int t = 0; t < enumInfo.count; t++)
		//{
		//	if (enumInfo.items[t].val == val.value._enum)
		//	{
		//		ret = enumInfo.items[t].name;
		//		break;
		//	}
		//}
		break;
	}
	case eIOType::Digital:
	{
		ret = CommonHelper::StringFormat1KB("%d", val.value._digital);
		break;
	}
	case eIOType::Analog:
	{
		ret = CommonHelper::StringFormat1KB("%f", val.value._analog);
		break;
	}
	case eIOType::String:
	{
		ret = val.value._string;
		break;
	}
	}
	return ret;
}

inline std::string VarIOToString2(sVarIO &val)
{
	std::string ret;
	switch (val.type)
	{
	case eIOType::Enum:
	{
		ret = CommonHelper::StringFormat1KB("%d", val.value._enum);
		break;
	}
	case eIOType::Digital:
	{
		ret = CommonHelper::StringFormat1KB("%d", val.value._digital);
		break;
	}
	case eIOType::Analog:
	{
		ret = CommonHelper::StringFormat1KB("%f", val.value._analog);
		break;
	}
	case eIOType::String:
	{
		ret = val.value._string;
		break;
	}
	}
	return ret;
}

// IO Write Log for tracing.
// Only write when the caller want to write log.
inline void IOWriteLog(SCLogger *logObj, const char *caller, const char *func, int line, sDriverWriteIO *targetValue, retval_ios result, const char *_msg)
{
	if (logObj == nullptr)
		return;

	static const char *nullchar = "NA";

	if (caller == nullptr)
		caller = nullchar;

	if (func == nullptr)
		func = nullchar;

	switch (targetValue->data.type)
	{
	case eIOType::Enum:
		logObj->log(SourceLoc{ "", 0}, LogLevel::trace, 0, ",%s,E,%s,%d,%s,%s,%d,%lld,%s", IOS_Convert(result), targetValue->info.fullName, targetValue->data.value._enum, caller, func, line, targetValue->timeStamp.time_since_epoch().count(), _msg);
		break;
	case eIOType::Digital:
		logObj->log(SourceLoc{ "", 0 }, LogLevel::trace, 0, ",%s,D,%s,%d,%s,%s,%d,%lld,%s", IOS_Convert(result), targetValue->info.fullName, targetValue->data.value._digital, caller, func, line, targetValue->timeStamp.time_since_epoch().count(), _msg);
		break;
	case eIOType::Analog:
		logObj->log(SourceLoc{ "", 0 }, LogLevel::trace, 0, ",%s,A,%s,%f,%s,%s,%d,%lld,%s", IOS_Convert(result), targetValue->info.fullName, targetValue->data.value._analog, caller, func, line, targetValue->timeStamp.time_since_epoch().count(), _msg);
		break;
	case eIOType::String:
		logObj->log(SourceLoc{ "", 0 }, LogLevel::trace, 0, ",%s,S,%s,%s,%s,%s,%d,%lld,%s", IOS_Convert(result), targetValue->info.fullName, targetValue->data.value._string, caller, func, line, targetValue->timeStamp.time_since_epoch().count(), _msg);
		break;
	}

}



