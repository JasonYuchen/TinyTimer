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

void printAllData(map<wstring, ProgramInfo> &ProgramMap);

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
	HANDLE keyIn = GetStdHandle(STD_INPUT_HANDLE);      //��ñ�׼�����豸���
	INPUT_RECORD keyRec;        //���������¼��ṹ��  
	DWORD res;      //���巵�ؼ�¼  

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

			/*����LastName������map��ʱProgramMap[LastName]�����Ĭ�Ϲ��캯���Զ�����ProgramInfo
			 *��Ĭ�Ϲ��캯����Name������""�մ�����˱���������SetNameȷ�����ַǿ�*/
			ProgramMap[LastName].SetName(LastName);    
			LastTime = CurrentTime;
			LastName = CurrentName;
		}

		//������Ӧ
		ReadConsoleInput(keyIn, &keyRec, 1, &res);      //��ȡ�����¼�  
		if (keyRec.EventType == KEY_EVENT)              //�����ǰ�¼��Ǽ����¼�  
		{
			if (keyRec.Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE && keyRec.Event.KeyEvent.bKeyDown == false) //��ǰ�¼��������ΪEsc���������ͷ�ʱ��Ӧ
			{
				GetLocalTime(&CurrentTime);             //���ʹ�õ����д���¼
				ProgramMap[LastName].InsertNewDuration(make_pair(LastTime, CurrentTime));
				ProgramMap[LastName].SetName(LastName);
				SetAbsLocate(0, 1);
				printAllData(ProgramMap);
				wcout << "End Time : ";
				PrintTime(wcout, LastTime);
				return 0;
			}
		}

		//�������
		Sleep(100);
	}
	return 0;
}

/*��ȡ��ǰ���еĽ�������
 *����ղ���������void
 *���ؽ������ƣ�����wstring*/
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

void printAllData(map<wstring, ProgramInfo> &ProgramMap)
{
	cout << "-----------------------------------------" << endl;
	for (auto ch : ProgramMap)
	{
		wcout << ch.second << endl;
	}
	cout << "-----------------------------------------" << endl;
}