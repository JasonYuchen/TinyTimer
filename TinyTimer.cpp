#include<iostream>
#include<iomanip>
#include<Windows.h>
#include<psapi.h>
#include<vector>
#include<map>
#include<string>
#include<utility>
#include"ProgramInfo.h"
#include<cassert>

using namespace std;

using namespace TinyTimer;

const int BUFSIZE = 255;

void SetAbsLocate(int x, int y)
{
	COORD coord;
	coord.X = x;        //从consolo左上角往右，x从0开始
	coord.Y = y;        //从consolo左上角往下，y从0开始
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
};

wstring getProgName();
wstring getWindowTitle();

int main()
{
	std::locale::global(std::locale(""));  //C++写法，注意这里，设置系统环境，处理中文IO，这是C的写法
	//初始化起始条件
	SYSTEMTIME currentTime{ 0 }, lastTime{ 0 };
	wstring lastName(getProgName());
	wstring lastTitle(getWindowTitle());
	GetLocalTime(&lastTime);

	map<wstring, Program> ProgramMap;
	ProgramMap.emplace(make_pair(lastName, Program(lastName)));
	HANDLE keyIn = GetStdHandle(STD_INPUT_HANDLE);      //获得标准输入设备句柄
	INPUT_RECORD keyRec;        //定义输入事件结构体  
	DWORD res;      //定义返回记录  
	wcout << lastName << " -> " << lastTitle << endl;
	while (true)
	{
		//通过封装Windows API来确定进程名及窗口标题
		wstring currentName(getProgName());
		wstring currentTitle(getWindowTitle());
		wcout << currentName << " -> " << currentTitle << endl;
		//根据名字判断是否是新程序或新窗口，及更新durations
		if (currentName != lastName)
		{
			GetLocalTime(&currentTime);
			if (ProgramMap.find(lastName) == ProgramMap.end())   
			{
				ProgramMap.emplace(make_pair(lastName, Program(lastName)));   //不加std::move会导致错误
			}
			if (ProgramMap.at(lastName).findView(lastTitle) == nullptr)
			{
				ProgramMap.at(lastName).addView(lastTitle);
			}
			ProgramMap.at(lastName).findView(lastTitle)->addDuration(lastTime, currentTime);
			lastTime = currentTime;
			lastName = currentName;
		}
		
		
		//键盘响应
		//ReadConsoleInput(keyIn, &keyRec, 1, &res);      //读取输入事件  
		//if (keyRec.EventType == KEY_EVENT)              //如果当前事件是键盘事件  
		//{
		//	if (keyRec.Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE && keyRec.Event.KeyEvent.bKeyDown == false) //当前事件的虚拟键为Esc键，且是释放时响应
		//	{
		//		wcout << "end" << endl;
		//		return 0;
		//	}
		//}
		
		//采样间隔
		Sleep(100);
	}
	return 0;
}

/*获取当前运行的进程名称
 *传入空参数，类型void
 *返回进程名称，类型wstring*/
wstring getProgName()
{
	DWORD PID;
	GetWindowThreadProcessId(GetForegroundWindow(), &PID);
	WCHAR name[MAX_PATH] = { 0 };
	HANDLE hPID = OpenProcess(PROCESS_ALL_ACCESS, 0, PID);
	GetModuleBaseName(hPID, NULL, name, sizeof(name));
	return wstring(name);
}

/*获取当前运行进程当前窗口名称
 *传入空参数，类型void
 *返回进程名称，类型wstring*/
wstring getWindowTitle()
{
	WCHAR title[BUFSIZE];
	GetWindowText(GetForegroundWindow(), title, sizeof(title));
	return wstring(title);
}