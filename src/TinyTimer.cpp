#include<iostream>
#include<vector>
#include<map>
#include<string>
#include<utility>
#include<algorithm>
#include<stdexcept>
#include<fstream>

#include"Program.h"
#include"PlatformAPI.h"

using namespace std;

using namespace TinyTimer;

int startTimer(map<wstring, Program> &ProgramMap);
void handleKeyboardEvent();

int main()
{
	std::locale::global(std::locale(""));
	map<wstring, Program> ProgramMap;
	while (true)
	{
		try
		{
			startTimer(ProgramMap);               //focus on the console and push "Esc" to stop and print all logs
		}
		catch (wstring msg)
		{
			if (msg == L"esc")
			{
				wcout << endl;
				std::for_each(ProgramMap.cbegin(), ProgramMap.cend(), [](decltype(*ProgramMap.cbegin()) it) { it.second.printAll(wcout); });
				break;
			}
			else if (msg == L"save")              //file IO
			{
				SYSTEMTIME now;
				GetLocalTime(&now);
				wstring filepath(to_wstring(now.wYear) + to_wstring(now.wMonth) + to_wstring(now.wDay));
				wofstream save(filepath + L".txt");
				std::for_each(ProgramMap.cbegin(), ProgramMap.cend(), [&](decltype(*ProgramMap.cbegin()) it) { it.second.printAll(save); });
				wcout << L"Save to " << filepath << L".txt completed!" << endl;
			}
		}
	}
	return 0;
}


int startTimer(map<wstring, Program> &ProgramMap)
{
	SYSTEMTIME currentTime{ 0 }, lastTime{ 0 };
	wstring lastName(getProgName());
	wstring cmd(lastName);
	wstring lastTitle(getWindowTitle());
	GetLocalTime(&lastTime);
	if (ProgramMap.empty())
	{
		ProgramMap.emplace(make_pair(lastName, Program(lastName)));
		wcout << "Start Time : " << lastTime << endl;
	}
	while (true)
	{

		wstring currentName(getProgName());
		wstring currentTitle(getWindowTitle());

		//when new program or new view is running, log the last program or view
		if (currentName != lastName && currentName != L"" || currentTitle != lastTitle && currentTitle != L"")
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
		if (keyRec.Event.KeyEvent.wVirtualKeyCode == 'S' && keyRec.Event.KeyEvent.bKeyDown == false)
		{
			throw(wstring(L"save"));
		}
	}
}