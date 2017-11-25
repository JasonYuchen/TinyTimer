#pragma once
#include<Windows.h>
#include<utility>
#include<set>
#include<string>

namespace ProgInfo
{
	using namespace std;        //在头文件的命名空间内使用using指示
	class ProgramInfo
	{
		friend wostream &operator<<(wostream &os, const ProgramInfo &rhs);
	public:
		using StartTime = SYSTEMTIME;
		using EndTime = SYSTEMTIME;
		using Value_type = pair<StartTime, EndTime>;
		//只允许拷贝/移动构造及wstring来构造，禁止赋值构造
		ProgramInfo() = default;  //不能delete，map中当key不存在时会调用元素的默认构造
		ProgramInfo(const ProgramInfo &rhs) :Name(rhs.Name), durations(rhs.durations) {}
		ProgramInfo(ProgramInfo &&rhs) noexcept : Name(std::move(rhs.Name)), durations(std::move(rhs.durations)) {}
		explicit ProgramInfo(const wstring &ws) : Name(ws), durations() {}
		ProgramInfo &operator=(const ProgramInfo &rhs) = delete;
		ProgramInfo &operator=(ProgramInfo &&rhs) = delete;
		~ProgramInfo() {}

		const wstring &GetName() const;                             //返回应用程序名字
		void SetName(const wstring &ws);                            //设置应用程序名字
		set<Value_type> &GetDurationSet();                          //返回时间集合
		const set<Value_type> &GetDurationSet() const;              
		set<Value_type>::size_type GetTotalTimes() const;           //返回总次数
		int GetTotalDuration() const;                               //返回总时长（秒）
		const StartTime &GetFirstRunTime() const;                   //返回第一次运行的时间点（SYSTEMTIME结构体）
		const StartTime &GetLastRunTime() const;                    //返回最后一次运行的时间点（SYSTEMTIME结构体）
		ProgramInfo &InsertNewDuration(const Value_type &rhs);      //添加一个时间段
		ProgramInfo &InsertNewDuration(Value_type &&rhs);
	private:
		wstring Name;
		set<Value_type> durations;       //set要求元素至少实现<运算，因此先简易实现了pair<StartTime, EndTime>之间的比较，基于时间
	};
	void PrintTime(wostream &os, const SYSTEMTIME &val);
	int GetDiffSeconds(const SYSTEMTIME &t1, const SYSTEMTIME &t2);
	bool operator==(const ProgramInfo &lhs, const ProgramInfo &rhs);
	bool operator!=(const ProgramInfo &lhs, const ProgramInfo &rhs);
	bool operator< (const ProgramInfo &lhs, const ProgramInfo &rhs);
}
