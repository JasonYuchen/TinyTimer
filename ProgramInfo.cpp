#include"ProgramInfo.h"
#include<iomanip>

namespace ProgInfo
{
	using Value_type = ProgramInfo::Value_type;
	bool operator==(const ProgramInfo &lhs, const ProgramInfo &rhs)
	{
		return lhs.GetName() == rhs.GetName();
	}

	bool operator!=(const ProgramInfo &lhs, const ProgramInfo &rhs)
	{
		return !(lhs == rhs);
	}

	bool operator< (const ProgramInfo &lhs, const ProgramInfo &rhs)
	{
		return lhs.GetName() < rhs.GetName();
	}

	void PrintTime(wostream &os, const SYSTEMTIME &val)
	{
		os << setfill(WCHAR('0')) << setw(2) << val.wHour << ":" << setw(2) << val.wMinute << ":" << setw(2) << val.wSecond << ends;
	}

	wostream &operator<<(wostream &os, const ProgramInfo &rhs)
	{
		os << rhs.GetName() << endl;
		for (auto ch : rhs.GetDurationSet())
		{
			PrintTime(os, ch.first);
			os << " ";
			PrintTime(os, ch.second);
			os << endl;
		}
		os << "Total Times = " << rhs.GetTotalTimes() << " Total Duration = " << rhs.GetTotalDuration() << "s" << endl;
		return os;
	}

	const wstring &ProgramInfo::GetName() const
	{
		return Name;
	}

	void ProgramInfo::SetName(const wstring &ws)
	{
		Name = ws;
	}

	set<Value_type> &ProgramInfo::GetDurationSet()
	{
		return durations;
	}

	const set<Value_type> &ProgramInfo::GetDurationSet() const
	{
		return durations;
	}

	set<Value_type>::size_type ProgramInfo::GetTotalTimes() const
	{
		return durations.size();
	}

	int GetDiffSeconds(const SYSTEMTIME &t1, const SYSTEMTIME &t2);
	int ProgramInfo::GetTotalDuration() const
	{
		int sum = 0;
		for (auto c : durations)
		{
			sum += GetDiffSeconds(c.first, c.second);
		}
		return sum;
	}

	const ProgramInfo::StartTime &ProgramInfo::GetFirstRunTime() const
	{
		return durations.cbegin()->first;
	}

	const ProgramInfo::StartTime &ProgramInfo::GetLastRunTime() const
	{
		return (--durations.cend())->first;
	}

	ProgramInfo &ProgramInfo::InsertNewDuration(const Value_type &rhs)
	{
		durations.insert(rhs);
		return *this;
	}

	ProgramInfo &ProgramInfo::InsertNewDuration(Value_type &&rhs)
	{
		durations.insert(std::move(rhs));
		return *this;
	}

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
}

bool operator< (const SYSTEMTIME &lhs, const SYSTEMTIME &rhs)
{
	time_t tt1 = ProgInfo::SystemtimeToTime_t(lhs);
	time_t tt2 = ProgInfo::SystemtimeToTime_t(rhs);
	return tt1 < tt2;
}