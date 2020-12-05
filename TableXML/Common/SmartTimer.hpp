#pragma once

#include <chrono>
#include <mutex>
#include <algorithm>
#include <vector>
#include <string>
/**
 @author  soohwan.cho(@semes.com)
 @brief  Time check for multiple events.
 @date  2020.03.10
 @example

	SmartTimer timer;

	// time check ..............

	timer.Start("Whole Time");
	
		timer.Start("Event A");
		// do something...
		timer.End(); // timer end at the last event "Event A"


		timer.Start("Event B");
		// do something...
		timer.End(); // timer end at the last event "Event B"

		timer.Start("Event C");
		// do something...
		timer.End(); // timer end at the last event "Event C"

	timer.End("Whole Time");	// End "Whole Time" Event

	// output .........

	cout<<timer.GetElapased_ms("Event B")<<endl; // "Event B" elapsed time

	cout<<timer.ToStringAll()<<endl; // show all elapsed time events


	

*/
class SmartTimer
{
//using namespace std::chrono;

private:
	struct __item__
	{
		__item__(/*const char* name*/)
		{
			Reset();
			//this->name = name;
		}
		void Reset()
		{
			activeated = finished = false;
			//name = "";
		}

		//std::string name;
		bool activeated;
		bool finished;
		std::chrono::time_point<std::chrono::steady_clock> start;
		std::chrono::time_point<std::chrono::steady_clock> end;
	};

	using my_pair = std::pair<std::string, __item__*>;
	using my_vector = std::vector<my_pair>;


	struct comp
	{
		comp(std::string const& s) : _s(s) { }

		bool operator () (my_pair const& p)
		{
			return (p.first == _s);
		}

		std::string _s;
	};


	my_vector container;
	std::mutex mtx;
	bool disabled;
	std::string title;
	std::string lastStartEventName;

	__item__ *GetItem(const std::string &eventName, bool createIfNoExist = false)
	{
		__item__ *p = nullptr;
		my_vector::iterator iter = std::find_if(container.begin(), container.end(), comp(eventName));

		if (iter == container.end())
		{
			if (createIfNoExist == false)
				return nullptr;

			if (container.size() > 1000) // limit
				return nullptr;

			p = new __item__(/*eventName.c_str()*/);
			container.push_back(my_pair(eventName, p));
		}
		else
		{
			p = iter->second;
		}
		return p;
	}
	template <typename T>
	double GetElapsed(const std::string &eventName)
	{
		if (disabled)
			return -1;

		auto item = GetItem(eventName, false);

		if (item == nullptr)
			return -1;

		if (item->activeated == false || item->finished == false)
			return -1;

		return std::chrono::duration<double, T>(item->end - item->start).count();
	}
public:
	SmartTimer()
	{
		disabled = false;
	}
	~SmartTimer()
	{
		Reset();
	}
	void Enable(bool enable)
	{
		disabled = !enable;
	}
	void SetTitle(std::string title)
	{
		this->title = title;
	}
	void Reset()
	{
		std::lock_guard<std::mutex> lock(mtx);

		while (container.size() > 0)
		{
			delete container[0].second;
			container.erase(container.begin());
		}
		container.clear();
	}
	void RemoveEvent(std::string eventName)
	{
		std::lock_guard<std::mutex> lock(mtx);

		my_vector::iterator iter = std::find_if(container.begin(), container.end(), comp(eventName));
		//auto iter = table.find(eventName);
		if (iter == container.end())
			return;

		delete iter->second;
		container.erase(iter);
	}
	void Start(std::string eventName)
	{
		if (disabled)
			return;

		std::lock_guard<std::mutex> lock(mtx);

		auto item = GetItem(eventName, true);
		if (item == nullptr)
			return;

		lastStartEventName = eventName;
		item->Reset();
		item->activeated = true;
		item->start = std::chrono::high_resolution_clock::now();
	}
	void End()
	{
		End(lastStartEventName);
	}
	void End(std::string eventName)
	{
		if (disabled)
			return;

		std::lock_guard<std::mutex> lock(mtx);

		auto _now = std::chrono::high_resolution_clock::now();
		auto item = GetItem(eventName, false);

		if (item == nullptr)
			return;

		if (item->activeated == false || item->finished)
			return;

		item->end = _now;
		item->finished = true;
	}
	double GetElapsed_us(std::string eventName)
	{
		if (disabled)
			return -1;
		std::lock_guard<std::mutex> lock(mtx);

		return GetElapsed<std::micro>(eventName);
	}
	double GetElapsed_ms(std::string eventName)
	{
		if (disabled)
			return -1;

		std::lock_guard<std::mutex> lock(mtx);

		return GetElapsed<std::milli>(eventName);
	}
	double GetElapsed_sec(std::string eventName)
	{
		if (disabled)
			return -1;

		std::lock_guard<std::mutex> lock(mtx);

		auto item = GetItem(eventName, false);

		if (item == nullptr)
			return -1;

		if (item->activeated == false || item->finished == false)
			return -1;

		return std::chrono::duration<double>(item->end - item->start).count();
	}
	std::string ToStringAll()
	{
		std::string ret;

		if (disabled)
			return ret;

		std::lock_guard<std::mutex> lock(mtx);

		if (container.size() == 0)
			return ret;

		if (title.length() > 0)
			ret = "[" + title + "]\n";

		for (auto iter : container)
		{
			if (iter.second->activeated && iter.second->finished)
			{
				double ms = GetElapsed<std::milli>(iter.first);
				ret += iter.first + ": " + std::to_string(ms) + " ms\n";
			}
		}
		return ret;
	}

};

class ScopeTimer
{
public:
	ScopeTimer(SmartTimer &parent, std::string eventName)
		: _parent(nullptr)
	{
		_parent = &parent;
		_eventName = eventName;
		_parent->Start(eventName);
	}
	~ScopeTimer()
	{
		_parent->End(_eventName);
	}
private:
	SmartTimer* _parent;
	std::string _eventName;
};