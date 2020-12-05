#pragma once
#include <Windows.h>
#include "RTOslLib.h"
#include "esLib.h"

namespace CommonHelper
{
	class ProcessMutex
	{
	public:

		HANDLE handle;
		std::string name;

		ProcessMutex()
		{

		}
		~ProcessMutex()
		{
			Close();
		}
		const char *GetName()
		{
			return name.c_str();
		}
		bool Open(const char *_name)
		{
			Close();

			handle = g_pRTOslLib->OslCreateMutex(NULL, FALSE, _name);
			if (handle == nullptr)
				handle = g_pRTOslLib->OslOpenMutex(NULL, FALSE, _name);

			if (nullptr == handle)
				return false;
			
			name = _name;
			return true;
		}
		bool Lock(DWORD timeover /*= INFINITE */, DWORD *result = nullptr)
		{
			if (handle == nullptr)
				return false;


			DWORD dw = g_pRTOslLib->OslWaitForSingleObject(handle, timeover);

			if(result)
				*result = dw;
		
			if (dw == WAIT_OBJECT_0)
				return true;

			return false;
		}
		void Unlock()
		{
			if (handle == nullptr)
				return;

			g_pRTOslLib->OslReleaseMutex(handle);
		}
		void Close()
		{
			Unlock();
		}
	};

	class MutexScopeLock
	{
		ProcessMutex *ptr;
		
		DWORD dwRet;
		bool locked;

	public:

		
		MutexScopeLock(ProcessMutex *mtx, DWORD timeout)
		{
			ptr = mtx;
			locked = ptr->Lock(timeout, &dwRet);
		}
		~MutexScopeLock()
		{
			ptr->Unlock();
			locked = false;
		}
		DWORD GetLockResult()
		{
			return dwRet;
		}
		bool IsLock()
		{
			return locked;
		}
	};
	
	/**
	 * @Details : You can use shared memory such as a normal value, This gives you  the interface of shared memory
	 * @Author : soohwan.cho(@semes.com)
	 * @Date : 2019.08.08
	*/
	
	/*
	SharedMemoryValue<int> mySma;
	mySma.Create("unique_name_must");
	mySma.Write(10);
	int val = mySma.Read();
	*/

	template <typename the_type>
	class SharedMemoryValue
	{
	private:

		std::string name;
		esLib::esSharedMemory sma;
		ProcessMutex mutex;

	public:

		SharedMemoryValue()
		{
		}
		~SharedMemoryValue()
		{
			sma.Close();	//automatically close
		}

		bool IsCreated()
		{
			return sma.addr != nullptr;
		}

		bool Create(const char *name)
		{
			this->name = name;

			char mutexName[MAX_PATH];
			sprintf_s(mutexName, "%s_MUTEX", name);
			//if (this->mutex.Create(mutexName) == false)
			if (this->mutex.Open(mutexName) == false)
				return false;

			return sma.Open(name, sizeof(the_type));

#if(false)
//			DWORD dwDesiredAccess = OptionChk(READONLY) ? PAGE_READONLY : PAGE_READWRITE;
//			this->hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, dwDesiredAccess, 0, sizeof(the_type), name);
//			if (this->hMapFile == NULL)
//			{
//				return false;
//			}
//
//			dwDesiredAccess = OptionChk(READONLY) ? FILE_MAP_READ : FILE_MAP_ALL_ACCESS;
//			this->pStartAddress = (the_type*)MapViewOfFile(this->hMapFile, dwDesiredAccess, 0, 0, 0);
//
//			if (this->pStartAddress == NULL)
//			{
//				return false;
//			}
//			return true;
#endif
		}

		the_type Read()
		{
			the_type pRetVal;
			memset(&pRetVal, NULL, sizeof(the_type));

			if (sma.addr == NULL)
				return pRetVal;

			mutex.Lock(100);
			memcpy(&pRetVal, sma.addr, sizeof(the_type));
			mutex.Unlock();

			return pRetVal;
		}

		bool Write(the_type val)
		{
			if (sma.addr == NULL)
				return false;

			mutex.Lock(100);
			memcpy(sma.addr, &val, sizeof(the_type));
			mutex.Unlock();
			return true;
		}

		std::string GetName() { return name; }	
	};
};