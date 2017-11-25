#pragma once
#include<Windows.h>
#include<utility>
#include<set>
#include<string>

namespace ProgInfo
{
	using namespace std;        //��ͷ�ļ��������ռ���ʹ��usingָʾ
	class ProgramInfo
	{
		friend wostream &operator<<(wostream &os, const ProgramInfo &rhs);
	public:
		using StartTime = SYSTEMTIME;
		using EndTime = SYSTEMTIME;
		using Value_type = pair<StartTime, EndTime>;
		//ֻ������/�ƶ����켰wstring�����죬��ֹ��ֵ����
		ProgramInfo() = default;  //����delete��map�е�key������ʱ�����Ԫ�ص�Ĭ�Ϲ���
		ProgramInfo(const ProgramInfo &rhs) :Name(rhs.Name), durations(rhs.durations) {}
		ProgramInfo(ProgramInfo &&rhs) noexcept : Name(std::move(rhs.Name)), durations(std::move(rhs.durations)) {}
		explicit ProgramInfo(const wstring &ws) : Name(ws), durations() {}
		ProgramInfo &operator=(const ProgramInfo &rhs) = delete;
		ProgramInfo &operator=(ProgramInfo &&rhs) = delete;
		~ProgramInfo() {}

		const wstring &GetName() const;                             //����Ӧ�ó�������
		void SetName(const wstring &ws);                            //����Ӧ�ó�������
		set<Value_type> &GetDurationSet();                          //����ʱ�伯��
		const set<Value_type> &GetDurationSet() const;              
		set<Value_type>::size_type GetTotalTimes() const;           //�����ܴ���
		int GetTotalDuration() const;                               //������ʱ�����룩
		const StartTime &GetFirstRunTime() const;                   //���ص�һ�����е�ʱ��㣨SYSTEMTIME�ṹ�壩
		const StartTime &GetLastRunTime() const;                    //�������һ�����е�ʱ��㣨SYSTEMTIME�ṹ�壩
		ProgramInfo &InsertNewDuration(const Value_type &rhs);      //���һ��ʱ���
		ProgramInfo &InsertNewDuration(Value_type &&rhs);
	private:
		wstring Name;
		set<Value_type> durations;       //setҪ��Ԫ������ʵ��<���㣬����ȼ���ʵ����pair<StartTime, EndTime>֮��ıȽϣ�����ʱ��
	};
	void PrintTime(wostream &os, const SYSTEMTIME &val);
	int GetDiffSeconds(const SYSTEMTIME &t1, const SYSTEMTIME &t2);
	bool operator==(const ProgramInfo &lhs, const ProgramInfo &rhs);
	bool operator!=(const ProgramInfo &lhs, const ProgramInfo &rhs);
	bool operator< (const ProgramInfo &lhs, const ProgramInfo &rhs);
}
