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
	coord.X = x;        //��consolo���Ͻ����ң�x��0��ʼ
	coord.Y = y;        //��consolo���Ͻ����£�y��0��ʼ
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
};

wstring GetCurrentName();
bool TestTime();

int main()
{
	//��ʼ����ʼ����
	SYSTEMTIME CurrentTime{ 0 }, LastTime{ 0 };
	wstring LastName = GetCurrentName();
	GetLocalTime(&LastTime);
	wcout << "Start Time : ";   //ע�⣺���ڿ��ַ���setfillҲҪ�Կ��ַ�Ϊʵ��
	PrintTime(wcout, LastTime);
	map<wstring, ProgramInfo> ProgramMap;
	ProgramMap.emplace(make_pair(LastName, ProgramInfo(LastName)));

	int i = 0;
	while (1)
	{
		//ͨ����װWindows API��ȷ�����ּ�ʱ��
		wstring CurrentName = GetCurrentName();
		//���������ж��Ƿ����³��򣬼�����durations
		if (CurrentName != LastName)
		{
			GetLocalTime(&CurrentTime);
			ProgramMap[LastName].InsertNewDuration(make_pair(LastTime, CurrentTime));
			ProgramMap[LastName].SetName(LastName);    //����LastName������map��ʱProgramMap[LastName]�����Ĭ�Ϲ��캯���Զ�����ProgramInfo����Ĭ�Ϲ��캯����Name������""�մ�����˱���������SetNameȷ�����ַǿ�
			LastTime = CurrentTime;
			LastName = CurrentName;
		}
		if (TestTime())
			break;
		//�������
		Sleep(1000);
	}
	GetLocalTime(&CurrentTime);  //���ʹ�õ����д���¼
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