#include<iostream>
#include<iomanip>
#include<Windows.h>
#include<psapi.h>
#include<vector>
#include<map>
#include<string>
#include<utility>
#include"ProgramInfo.h"
#include<algorithm>
#include<stdexcept>

using namespace std;

using namespace TinyTimer;

const int BUFSIZE = 255;

wstring getProgName();
wstring getWindowTitle();
void handleKeyboardEvent();
int startTimer(map<wstring, Program> &ProgramMap);

int main()
{
	std::locale::global(std::locale(""));
	map<wstring, Program> ProgramMap;
	try
	{
		startTimer(ProgramMap);               //focus on the console and push "Esc" to stop and print all logs
	}
	catch (wstring msg)
	{
		wcout << msg << endl;
		if (msg == L"esc")
		{
			std::for_each(ProgramMap.cbegin(), ProgramMap.cend(), [](decltype(*ProgramMap.cbegin()) it) { it.second.printAll(); });
		}
	}
	

	return 0;
}

void SetAbsLocate(int x, int y)
{
	COORD coord;
	coord.X = x;
	coord.Y = y;  
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
};

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

void handleKeyboardEvent()
{
	HANDLE keyIn = GetStdHandle(STD_INPUT_HANDLE);
	INPUT_RECORD keyRec;
	DWORD res;
	ReadConsoleInput(keyIn, &keyRec, 1, &res);
	if (keyRec.EventType == KEY_EVENT)
	{
		if (keyRec.Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE && keyRec.Event.KeyEvent.bKeyDown == false)
		{
			throw(wstring(L"esc"));
		}
	}
}

int startTimer(map<wstring, Program> &ProgramMap)
{
	SYSTEMTIME currentTime{ 0 }, lastTime{ 0 };
	wstring lastName(getProgName());
	wstring cmd(lastName);
	wstring lastTitle(getWindowTitle());
	GetLocalTime(&lastTime);

	ProgramMap.emplace(make_pair(lastName, Program(lastName)));
	wcout << "Start Time : " << lastTime << endl;
	while (true)
	{

		wstring currentName(getProgName());
		wstring currentTitle(getWindowTitle());
		SetAbsLocate(0, 1);
		wcout << "Current Time : " << currentTime << endl;

		//when new program or new view is running, log the last program or view
		if (currentName != lastName && currentName != L"")
		{
			GetLocalTime(&currentTime);
			if (ProgramMap.find(lastName) == ProgramMap.end())
			{
				ProgramMap.emplace(make_pair(lastName, Program(lastName)));
			}
			if (ProgramMap.at(lastName).findView(lastTitle) == nullptr)   //use at() to avoid the side effect of [], at() also have parameter check
			{
				ProgramMap.at(lastName).addView(lastTitle);
			}
			ProgramMap.at(lastName).findView(lastTitle)->addDuration(lastTime, currentTime);
			lastTime = currentTime;
			lastName = currentName;
			lastTitle = currentTitle;
		}

		//when focus on TinyTimer, prepare for keyboard event. However this stops the timer, which will be sovled by multithread laterly
		if (currentName == cmd)
		{
			handleKeyboardEvent();
		}
		Sleep(100);
	}
	return 0;
}