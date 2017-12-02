#pragma once
#include<vector>
#include<memory>
#include<string>
#include<Windows.h>
#include<utility>
#include<set>

namespace TinyTimer
{
	using std::shared_ptr;
	using std::wstring;
	using std::pair;
	using std::set;
	class Window;
	class Program
	{
	public:
		Program() = delete;
		Program(const Program &) = delete;
		Program(Program &&rhs) :name(std::move(rhs.name)), views(std::move(rhs.views)) {}
		Program &operator=(const Program &) = delete;
		Program &operator=(Program &&rhs) { name = std::move(rhs.name); views = std::move(rhs.views); }
		explicit Program(const wstring &rhs) :name(rhs), views() {}
		~Program() {}

		shared_ptr<Window> findView(const wstring &rhs);
		Program &addView(const wstring &rhs);
		wstring getName() const { return name; }
		size_t totalTimes() const;
		size_t totalDuration() const;
		void printAll() const;
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
		Window(Window &&rhs) :title(std::move(rhs.title)), prog(std::move(rhs.prog)), durations(std::move(rhs.durations)) {}
		Window &operator=(const Window &) = delete;
		Window &operator=(Window &&rhs) { title = std::move(rhs.title); prog = std::move(rhs.prog); durations = std::move(rhs.durations); }
		explicit Window(const wstring &rhs, const Program *p) :title(rhs), prog(p), durations() {}
		~Window() {}

		wstring getTitle() const { return title; }
		wstring getProgName() const { return prog->getName(); }
		Window &addDuration(const SYSTEMTIME &lhs, const SYSTEMTIME &rhs) { durations.emplace(std::make_pair(lhs, rhs)); return *this; }
		size_t totalTimes() const;
		size_t totalDuration() const;
		void printAll() const;
	private:
		wstring title;
		const Program *prog;                    //cannot use shared_ptr/unique_ptr, which would cause circular reference
		set<Value_type> durations;
	};
	bool operator< (const Program &lhs, const Program &rhs);
	bool operator< (const Window &lhs, const Window &rhs);
	int GetDiffSeconds(const SYSTEMTIME &t1, const SYSTEMTIME &t2);
}

bool operator< (const SYSTEMTIME &lhs, const SYSTEMTIME &rhs);
std::wostream &operator<<(std::wostream &os, const SYSTEMTIME &rhs);