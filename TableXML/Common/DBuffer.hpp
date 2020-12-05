#pragma once
//#include <windows.h>
#include <string>
#include "UserException.hpp"

#ifndef __MIN__
#define __MIN__(a, b)  (((a) < (b)) ? (a) : (b))
#endif

#ifndef __MAX__
#define __MAX__(a, b)  (((a) < (b)) ? (b) : (a))
#endif

/**
* @breif:  common memory buffer class. Header file only.
* @date: 2019.09.05
* @author: soohwan.cho(@semse.com)
*/
class DBuffer
{
	unsigned char *data;
public:
	static const int LIMIT_SIZE = 10000000;	//10MB

	int allocSize;
	int length;
	int pointer;

	DBuffer() 
	{ 
		memset(this, NULL, sizeof(*this)); 
	}

	~DBuffer()
	{
		Clear();
	}

	void Clear()
	{
		Init();
		if (data)
		{
			delete[]data;
			data = nullptr;
		}
		allocSize = pointer = length = 0;
	}

	void Init(bool doZeromem = false)
	{
		length = pointer = 0;
		if (doZeromem)
			memset(data, NULL, allocSize);
	}

	bool Create(int _size)
	{
		if (data)
			return false;

		if (_size <= 0 || _size > LIMIT_SIZE)
			return false;

		data = new (std::nothrow) unsigned char[_size];
		if (data == nullptr)
			return false;

		allocSize = _size;
		pointer = 0;
		length = 0;
		return true;
	}
	void SeekSet()
	{
		pointer = 0;
	}
	bool SeekOffset(int offset)
	{
		if (pointer + offset > allocSize)
			return false;
		pointer += offset;
		return true;
	}
	void SeekLast()
	{
		pointer = length;
	}

	void AddLength(int add)
	{
		length += add;
	}
	unsigned char *DataPtr() 
	{
		return data + pointer;
	}
	unsigned char *DataPtr0()
	{
		return data;
	}
	bool AppendByte(unsigned char val)
	{
		if (pointer == allocSize || length == allocSize)
			return false;
		data[pointer++] = val;
		length++;
		return true;
	}
	void* CopyFrom(const void *src, int _size, bool useMemmove = false, bool addLength = true, const char *fnc = nullptr, int line = 0)
	{
		if (length + _size > allocSize)
		{
			if(fnc)
				throw UserException(_FN, _LN, 0, nullptr, "out of range. %s()[%d] pos:%d, size:%d, copy:%d", fnc, line, pointer, allocSize, _size);
			else
				throw UserException(_FN, _LN, 0, nullptr, "out of range. pos:%d, size:%d, copy:%d", pointer, allocSize, _size);
			
		}
		void *addr = data + pointer;
		try
		{
			if (useMemmove)
				memmove_s(data + pointer, allocSize - pointer, src, _size);
			else
				memcpy_s(data + pointer, allocSize - pointer, src, _size);
		}
		catch (...)
		{
			throw UserException(_FN, _LN, 0, nullptr, "memory %s failed. pos:%d, size:%d, copy:%d", useMemmove ? "move" : "copy", pointer, allocSize, _size);
		}

		pointer += _size;
		length += _size;
		return addr;
	}
	int GetLeftSize()
	{
		return length - pointer;
	}
	void CopyTo(void *dst, int _dstSize, bool useMemmove = false)
	{
		//if ( _dstSize < length - pointer)
		//	throw UserException(_FN, _LN, 0, nullptr, "out of range. pos:%d, size:%d, copy:%d", pointer, allocSize, _dstSize);

		int copySize = __MIN__(length - pointer, _dstSize);
		try
		{
			if (useMemmove)
				memmove_s(dst, _dstSize, data + pointer, copySize);
			else
				memcpy_s(dst, _dstSize, data + pointer, copySize);
		}
		catch (...)
		{
			throw UserException(_FN, _LN, 0, nullptr, "memory %s failed. pos:%d, size:%d, copy:%d", useMemmove ? "move" : "copy", pointer, allocSize, _dstSize);
		}
		pointer += copySize;
	}
	void CopyTo_t(void *dst, int _dstSize, bool useMemmove = false, const char *fnc = nullptr, int line = 0)
	{
		if (_dstSize > length - pointer)
		{
			if(fnc)
				throw UserException(_FN, _LN, 0, nullptr, "out of range. %s()[%d] pos:%d, size:%d, copy:%d", fnc, line, pointer, allocSize, _dstSize);
			else
				throw UserException(_FN, _LN, 0, nullptr, "out of range. pos:%d, size:%d, copy:%d", pointer, allocSize, _dstSize);
		}

		try
		{
			if (useMemmove)
				memmove_s(dst, _dstSize, data + pointer, _dstSize);
			else
				memcpy_s(dst, _dstSize, data + pointer, _dstSize);
		}
		catch (...)
		{
			throw UserException(_FN, _LN, 0, nullptr, "memory %s failed. pos:%d, size:%d, copy:%d", useMemmove ? "move" : "copy", pointer, allocSize, _dstSize);
		}
		pointer += _dstSize;
	}
};
