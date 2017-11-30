#include"ProgramInfo.h"
#include<iomanip>

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
	bool operator< (const View &lhs, const View &rhs)
	{
		return lhs.getTitle() < rhs.getTitle();
	}
	shared_ptr<View> Program::findView(const wstring &rhs)
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
		views.emplace(shared_ptr<View>(new View(rhs, this))); 
		return *this;
	}
}
bool operator< (const SYSTEMTIME &lhs, const SYSTEMTIME &rhs)  //SYSTEMTIME is a global class, thus its operator should be global
{
	time_t tt1 = TinyTimer::SystemtimeToTime_t(lhs);
	time_t tt2 = TinyTimer::SystemtimeToTime_t(rhs);
	return tt1 < tt2;
}