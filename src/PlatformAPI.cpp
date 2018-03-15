#include"PlatformAPI.h"
#include<string>
#include<Windows.h>
#include<psapi.h>
#include<iostream>
#include<iomanip>

using namespace std;

const int BUFSIZE = 255;

wstring getProgName()
{
	DWORD PID;
	GetWindowThreadProcessId(GetForegroundWindow(), &PID);
	WCHAR name[MAX_PATH] = { 0 };
	HANDLE hPID = OpenProcess(PROCESS_ALL_ACCESS, 0, PID);
	GetModuleBaseName(hPID, NULL, name, sizeof(name));
	return wstring(name);
}
wstring getWindowTitle()
{
	WCHAR title[BUFSIZE];
	GetWindowText(GetForegroundWindow(), title, sizeof(title));
	return wstring(title);
}

void SetAbsLocate(int x, int y)
{
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
};

std::wostream &operator<<(std::wostream &os, const SYSTEMTIME &rhs)
{
	using std::setw;
	using std::setfill;
	os << setfill(L'0') << setw(2) << rhs.wHour << ":" << setw(2) << rhs.wMinute << ":" << setw(2) << rhs.wSecond << std::ends;
	return os;
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

bool operator< (const SYSTEMTIME &lhs, const SYSTEMTIME &rhs)
{
	time_t tt1 = SystemtimeToTime_t(lhs);
	time_t tt2 = SystemtimeToTime_t(rhs);
	return tt1 < tt2;
}

int GetDiffSeconds(const SYSTEMTIME &t1, const SYSTEMTIME &t2)
{
	time_t tt1 = SystemtimeToTime_t(t1);
	time_t tt2 = SystemtimeToTime_t(t2);
	return static_cast<int>(tt2 - tt1);
}