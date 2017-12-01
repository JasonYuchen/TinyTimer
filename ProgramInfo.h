#pragma once
#include<vector>
#include<memory>
#include<string>
#include<Windows.h>
#include<utility>
#include<set>

namespace TinyTimer
{
	using namespace std;
	class View;
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

		shared_ptr<View> findView(const wstring &rhs);
		Program &addView(const wstring &rhs);
		wstring getName() const { return name; }
		void printAll() const;
	private:
		wstring name;
		set<shared_ptr<View>> views;
	};

	class View
	{
	public:
		using Value_type = pair<SYSTEMTIME, SYSTEMTIME>;
		View() = delete;
		View(const View &) = delete;
		View(View &&rhs) :title(std::move(rhs.title)), prog(std::move(rhs.prog)), durations(std::move(rhs.durations)) {}
		View &operator=(const View &) = delete;
		View &operator=(View &&rhs) { title = std::move(rhs.title); prog = std::move(rhs.prog); durations = std::move(rhs.durations); }
		explicit View(const wstring &rhs, const Program *p) :title(rhs), prog(p), durations() {}
		~View() {}

		wstring getTitle() const { return title; }
		wstring getProgName() const { return prog->getName(); }
		View &addDuration(const SYSTEMTIME &lhs, const SYSTEMTIME &rhs) { durations.emplace(make_pair(lhs, rhs)); return *this; }
		void printAll() const;
	private:
		wstring title;
		const Program *prog;                    //cannot use shared_ptr/unique_ptr, which would cause circular reference
		set<Value_type> durations;
	};
	bool operator< (const Program &lhs, const Program &rhs);
	bool operator< (const View &lhs, const View &rhs);
	int GetDiffSeconds(const SYSTEMTIME &t1, const SYSTEMTIME &t2);
}

bool operator< (const SYSTEMTIME &lhs, const SYSTEMTIME &rhs);