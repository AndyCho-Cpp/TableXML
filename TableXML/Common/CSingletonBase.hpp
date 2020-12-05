#pragma once
#include <string>
//#include <Windows.h>
//#include <synchapi.h>	// for critical_section
//#include "esLib.h"
#include <mutex>
/**
* @brief: CSingletonBase class is singleton interface. once you inherit, it is able to remove the chores to make sington class.
* and this offers a union way for sington
* @Usage: You must write THREE defines beforehand (1), (2), (3)

// header file
class MyClass : public CSingletonBase<MyClass>
{
DECLARE_SINGLETON(MyClass)            . . . . . . . . . . . (1)
...

// cpp file (top of file, below include)
#include "MyClass.h"

IMPLEMENT_SINGLETON(MyClass)		 . . . . . . . . . (2)
...

virtual void Destroy() override				 . . . . . . . . . (3)
{
// todo
initialized = false;
}

* @date: 2019.09.14
* @author: soohwan.cho(@semes.com)
*/

#define IMPLEMENT_SINGLETON(YourClass)	\
														YourClass* CSingletonBase<YourClass>::m_instance = nullptr;		\
														std::mutex CSingletonBase<YourClass>::m_instanceMutex;					


#define DECLARE_SINGLETON(YourClass)		friend class CSingletonBase<YourClass>;



template <typename the_type>
class CSingletonBase
{
protected:
	CSingletonBase()
		: initialized(false)
	{
		//m_mtx.Open();
	}

	~CSingletonBase()
	{
	}

	static the_type *m_instance;
	//static std::mutex instMutex;

	bool initialized;
	//esLib::esMutex m_mtx;
	
private:
	static std::mutex m_instanceMutex;

public:
	virtual void Destroy() = 0;
	
	static the_type &Inst()
	{
		if (m_instance == nullptr)
		{
			m_instanceMutex.lock();
			if (m_instance == nullptr)
			{
				static the_type inst;
				m_instance = &inst;
			}
			m_instanceMutex.unlock();
		}
		return *m_instance;
	}

	bool IsInitialized()
	{
		return initialized;
	}

};
