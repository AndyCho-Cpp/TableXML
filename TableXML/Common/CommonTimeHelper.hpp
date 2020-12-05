/**
* @author  soohwan.cho
* @date  2018.04.16
* @breif  All code was developed by soohwan.cho
*/

#pragma once
#include "CommonDefine.hpp"
#include "CommonStringHelper.hpp"
#include <chrono>
#include <Windows.h>
//#ifndef UNDER_RTSS
//#pragma comment(lib, "winmm.lib")
//#endif

namespace CommonHelper
{
	using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;

	class StdClock
	{
	public:
		static TimePoint Now_t() { return std::chrono::high_resolution_clock::now(); }	//now timepoint
		static __int64 Now_i() { return Now_t().time_since_epoch().count(); } //now integer

//#ifndef UNDER_RTSS
//		static DWORD Now_tick() { return timeGetTime(); } //now integer
//#else
//		static DWORD Now_tick() { return GetTickCount(); } //now integer
//#endif
		static __int64 Convert_ttoi(TimePoint time_point) { return time_point.time_since_epoch().count(); }
		static double Elapsed_ms(TimePoint &start, TimePoint end = Now_t()) { return std::chrono::duration<double, std::milli>(end - start).count(); }
		static double Elapsed_us(TimePoint &start, TimePoint end = Now_t()) { return std::chrono::duration<double, std::micro>(end - start).count(); }
		static double Elapsed_ns(TimePoint &start, TimePoint end = Now_t()) { return std::chrono::duration<double, std::nano>(end - start).count(); }
		static void Elapsed_DayHourMinSec(TimePoint &start, TimePoint end, int *out_Day, int *out_Hour, int *out_Min, int *out_Sec)
		{
			double ms = StdClock::Elapsed_ms(start, end);
			unsigned long runningTick = static_cast<unsigned long>(ms);

			const unsigned long aSec = 1000;
			const unsigned long aMin = 1000 * 60;
			const unsigned long aHour = 1000 * 60 * 60;
			const unsigned long aDay = 1000 * 60 * 60 * 24;

			int Days = runningTick / aDay;
			runningTick -= (Days * aDay);

			int Hours = runningTick / aHour;
			runningTick -= (Hours * aHour);

			int Mins = runningTick / aMin;
			runningTick -= (Mins * aMin);

			int Seconds = runningTick / aSec;
			runningTick -= (Seconds * aSec);

			if (out_Day)
				*out_Day = Days;
			if (out_Hour)
				*out_Hour = Hours;
			if (out_Min)
				*out_Min = Mins;
			if (out_Sec)
				*out_Sec = Seconds;
		}

		static SYSTEMTIME Time()
		{
			SYSTEMTIME tm;
			GetLocalTime(&tm);
			return tm;
		}
		
		// YYYY-MM-DD  (ISO8601 Format)
		static std::string StrDate(SYSTEMTIME *tm = nullptr)
		{
			SYSTEMTIME __now = Time();
			if (tm)
				__now = *tm;
			return  StringFormat1KB("%04d-%02d-%02d", __now.wYear, __now.wMonth, __now.wDay);
		}
		// YYYYMMDD
		static std::string StrDate2(SYSTEMTIME *tm = nullptr)
		{
			SYSTEMTIME __now = Time();
			if (tm)
				__now = *tm;
			return  StringFormat1KB("%04d%02d%02d", __now.wYear, __now.wMonth, __now.wDay);
		}
		// HH:MM:SS  (ISO8601 Format)
		static std::string StrTime(SYSTEMTIME *tm = nullptr)
		{
			SYSTEMTIME __now = Time();
			if (tm)
				__now = *tm;

			return StringFormat1KB("%02d:%02d:%02d", __now.wHour, __now.wMinute, __now.wSecond);
		}
		// HH:MM:SS.mmm
		static std::string StrTime2(SYSTEMTIME *tm = nullptr)
		{
			SYSTEMTIME __now = Time();
			if (tm)
				__now = *tm;

			return StringFormat1KB("%02d:%02d:%02d.%03d", __now.wHour, __now.wMinute, __now.wSecond, __now.wMilliseconds);
		}
	};
};