#include<iostream>
#include<iomanip>
#include<Windows.h>
#include<psapi.h>
#include<vector>
#include<map>
#include<string>
#include<utility>
#include"ProgramInfo.h"

using namespace std;
using ProgInfo::ProgramInfo;
using ProgInfo::PrintTime;
using ProgInfo::GetDiffSeconds;

void SetAbsLocate(int x, int y)
{
	COORD coord;
	coord.X = x;        //从consolo左上角往右，x从0开始
	coord.Y = y;        //从consolo左上角往下，y从0开始
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
};

wstring GetCurrentName();
bool TestTime();

int main()
{
	//初始化起始条件
	SYSTEMTIME CurrentTime{ 0 }, LastTime{ 0 };
	wstring LastName = GetCurrentName();
	GetLocalTime(&LastTime);
	wcout << "Start Time : ";   //注意：对于宽字符，setfill也要以宽字符为实参
	PrintTime(wcout, LastTime);
	map<wstring, ProgramInfo> ProgramMap;
	ProgramMap.emplace(make_pair(LastName, ProgramInfo(LastName)));

	int i = 0;
	while (1)
	{
		//通过封装Windows API来确定名字及时间
		wstring CurrentName = GetCurrentName();
		//根据名字判断是否是新程序，及更新durations
		if (CurrentName != LastName)
		{
			GetLocalTime(&CurrentTime);
			ProgramMap[LastName].InsertNewDuration(make_pair(LastTime, CurrentTime));
			ProgramMap[LastName].SetName(LastName);    //由于LastName不存在map中时ProgramMap[LastName]会调用默认构造函数自动生成ProgramInfo，而默认构造函数对Name会生成""空串，因此必须额外加上SetName确保名字非空
			LastTime = CurrentTime;
			LastName = CurrentName;
		}
		if (TestTime())
			break;
		//采样间隔
		Sleep(1000);
	}
	GetLocalTime(&CurrentTime);  //最后使用的软件写入记录
	ProgramMap[LastName].InsertNewDuration(make_pair(LastTime, CurrentTime));
	ProgramMap[LastName].SetName(LastName);
	SetAbsLocate(0, 1);
	cout << "-----------------------------------------" << endl;
	for (auto ch : ProgramMap)
	{
		wcout << ch.second << endl;
	}
	return 0;
}

wstring GetCurrentName()
{
	DWORD PID;
	GetWindowThreadProcessId(GetForegroundWindow(), &PID);
	WCHAR Path[MAX_PATH] = { 0 };
	HANDLE hPID = OpenProcess(PROCESS_ALL_ACCESS, 0, PID);
	GetModuleBaseName(hPID, NULL, Path, sizeof(Path));
	wstring CurrentName(Path);
	return CurrentName;
}


bool TestTime()
{
	SYSTEMTIME tmp{ 0 }, trg{ 0 };
	GetLocalTime(&tmp);
	trg = tmp;
	trg.wHour = 15;
	trg.wMinute = 58;
	SetAbsLocate(0, 1);
	wcout << GetDiffSeconds(tmp, trg) << "s remaining." << ends;
	if (tmp.wHour == trg.wHour && tmp.wMinute == trg.wMinute)
		return true;
	else
		return false;
}