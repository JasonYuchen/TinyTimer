#include"ProgramInfo.h"
#include<iostream>
#include<iomanip>
#include<algorithm>

namespace TinyTimer
{
	time_t FileTimeToTime_t(const FILETIME &ft)
	{
		ULARGE_INTEGER ui;
		ui.LowPart = ft.dwLowDateTime;
		ui.HighPart = ft.dwHighDateTime;
		return (static_cast<time_t>(ui.QuadPart - 116444736000000000) / 10000000);
	}

	time_t SystemtimeToTime_t(const SYSTEMTIME &t)
	{
		FILETIME fTime = { 0,0 };
		SystemTimeToFileTime(&t, &fTime);
		time_t tt = FileTimeToTime_t(fTime);
		return tt;
	}

	int GetDiffSeconds(const SYSTEMTIME &t1, const SYSTEMTIME &t2)
	{
		time_t tt1 = SystemtimeToTime_t(t1);
		time_t tt2 = SystemtimeToTime_t(t2);
		return static_cast<int>(tt2 - tt1);
	}
	bool operator< (const Program &lhs, const Program &rhs)
	{
		return lhs.getName() < rhs.getName();
	}
	bool operator< (const Window &lhs, const Window &rhs)
	{
		return lhs.getTitle() < rhs.getTitle();
	}
	shared_ptr<Window> Program::findView(const wstring &rhs)
	{
		for (auto v : views)
		{
			if (v->getTitle() == rhs)
				return v;
		}
		return nullptr;
	}
	Program &Program::addView(const wstring &rhs) 
	{ 
		views.emplace(shared_ptr<Window>(new Window(rhs, this))); 
		return *this;
	}
	size_t Window::totalTimes() const
	{
		return durations.size();
	}
	size_t Window::totalDuration() const
	{
		size_t ret = 0;
		std::for_each(durations.cbegin(), durations.cend(), [&ret](decltype(*durations.cbegin()) it) { ret += GetDiffSeconds(it.first, it.second); });
		return ret;
	}
	void Window::printAll() const
	{
		using std::wcout;
		using std::endl;
		wcout << title << endl;
		int time = 0;
		for (auto i : durations)
		{
			int tmp = GetDiffSeconds(i.first, i.second);
			wcout << i.first << " " << i.second << " " << tmp << "s" << endl;
			time += tmp;
		}
		wcout << "Window Times  = " << durations.size() << " Window Duration  = " << time << "s" << endl;
		wcout << "----------------------------------------" << endl;
	}
	size_t Program::totalTimes() const
	{
		size_t ret = 0;
		std::for_each(views.cbegin(), views.cend(), [&ret](decltype(*views.cbegin()) it) { ret += it->totalTimes(); });
		return ret;
	}
	size_t Program::totalDuration() const
	{
		size_t ret = 0;
		std::for_each(views.cbegin(), views.cend(), [&ret](decltype(*views.cbegin()) it) { ret += it->totalDuration(); });
		return ret;
	}
	void Program::printAll() const
	{
		using std::wcout;
		using std::endl;
		wcout << name << endl;
		wcout << "----------------------------------------" << endl;
		for (auto i : views)
		{
			i->printAll();
		}
		wcout << "Program Times = " << totalTimes() << " Program Duration = " << totalDuration() << "s" << endl;
		wcout << endl;
	}
}
bool operator< (const SYSTEMTIME &lhs, const SYSTEMTIME &rhs)  //SYSTEMTIME is a global class, thus its operator should be global
{
	time_t tt1 = TinyTimer::SystemtimeToTime_t(lhs);
	time_t tt2 = TinyTimer::SystemtimeToTime_t(rhs);
	return tt1 < tt2;
}
std::wostream &operator<<(std::wostream &os, const SYSTEMTIME &rhs)
{
	using std::setw;
	using std::setfill;
	os << setfill(L'0') << setw(2) << rhs.wHour << ":" << setw(2) << rhs.wMinute << ":" << setw(2) << rhs.wSecond << std::ends;
	return os;
}