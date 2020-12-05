#pragma once
#include <windows.h>
#include "RTOslLib.h"

namespace CommonHelper
{
	class ThreadHandle
	{
	private:
		HANDLE handle;
		HANDLE hKillSignal;
		HANDLE hResumeSignal;
	public:

		ThreadHandle()
		{
			memset(this, NULL, sizeof(*this));
			hKillSignal = OSL_CreateEvent(NULL, false, false, NULL);
			hResumeSignal = OSL_CreateEvent(NULL, true, true, NULL);		//normal true(set)
		}
		ThreadHandle(HANDLE h)
		{
			memset(this, NULL, sizeof(*this));
			hKillSignal = OSL_CreateEvent(NULL, false, false, NULL);
			hResumeSignal = OSL_CreateEvent(NULL, true, true, NULL);		//normal true(set)
			SetThreadHandle(h);
		}

		~ThreadHandle()
		{
			KillThread();
			CloseHandle(hResumeSignal);
			memset(this, NULL, sizeof(*this));
		}

		bool SetThreadHandle(HANDLE h)
		{
			if (handle)
				return false;

			handle = h;			
			return true;
		}
		void ResumeThread()
		{
			if(hResumeSignal)
				SetEvent(hResumeSignal);
		}
		void SuspendThread()
		{
			if(hResumeSignal)
				ResetEvent(hResumeSignal);
		}

		bool RunCheck(DWORD threadInterval_sleep_ms)
		{
			if (hKillSignal == NULL)
				return false;

			bool bRet = (OSL_WaitForSingleObject(hKillSignal, threadInterval_sleep_ms) == WAIT_TIMEOUT);
			if (bRet == false)
				return false;

			// suspend & resume
			OSL_WaitForSingleObject(hResumeSignal, INFINITE);
				
			return true;
		}
		// block function
		bool WaitUntilEnd(DWORD timeout_ms)
		{
			if (OSL_WaitForSingleObject(handle, timeout_ms) == WAIT_TIMEOUT)
				return false;

			return true;
		}
		bool KillThread(bool forceKill = false, DWORD dwTimeout = 3000)
		{
			if (hKillSignal == NULL || handle == nullptr)
				return true;

			ResumeThread();

			try
			{
				OSL_SetEvent(hKillSignal);

				if (OSL_WaitForSingleObject(handle, dwTimeout) == WAIT_TIMEOUT)
				{
					// exception case. force to close thread
					if (forceKill)
						OSL_TerminateThread(handle, 1);
					else
						return false;
				}
				OSL_CloseHandle(handle);
			}
			catch (...) {}
			
			handle = NULL;


			try
			{
				OSL_CloseHandle(hKillSignal);
			}
			catch (...) {}

			hKillSignal = NULL;
			return true;
		}
	};

};