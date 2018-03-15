#pragma once
#include<vector>
#include<memory>
#include<string>
#include<Windows.h>
#include<utility>
#include<set>
#include<iostream>

namespace TinyTimer
{
	using std::shared_ptr;
	using std::wstring;
	using std::pair;
	using std::set;
	using std::wostream;
	class Window;
	class Program
	{
	public:
		Program() = delete;
		Program(const Program &) = delete;
		Program(Program &&rhs);
		Program &operator=(const Program &) = delete;
		Program &operator=(Program &&rhs);
		explicit Program(const wstring &rhs);
		~Program();

		shared_ptr<Window> findView(const wstring &rhs);
		Program &addView(const wstring &rhs);
		wstring getName() const;
		size_t totalTimes() const;
		size_t totalDuration() const;
		wostream &printAll(wostream &os) const;
	private:
		wstring name;
		set<shared_ptr<Window>> views;
	};

	class Window
	{
	public:
		using Value_type = pair<SYSTEMTIME, SYSTEMTIME>;
		Window() = delete;
		Window(const Window &) = delete;
		Window(Window &&rhs);
		Window &operator=(const Window &) = delete;
		Window &operator=(Window &&rhs);
		explicit Window(const wstring &rhs, const Program *p);
		~Window();

		wstring getTitle() const;
		wstring getProgName() const;
		Window &addDuration(const SYSTEMTIME &lhs, const SYSTEMTIME &rhs);
		size_t totalTimes() const;
		size_t totalDuration() const;
		wostream &printAll(wostream &os) const;
	private:
		wstring title;
		const Program *prog;                    //cannot use shared_ptr/unique_ptr, which would cause circular reference
		set<Value_type> durations;
	};
	bool operator< (const Program &lhs, const Program &rhs);
	bool operator< (const Window &lhs, const Window &rhs);
}
