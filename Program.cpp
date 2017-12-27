#include<iostream>
#include<iomanip>
#include<algorithm>
#include<string>

#include"Program.h"
#include"PlatformAPI.h"

namespace TinyTimer
{
	bool operator< (const Program &lhs, const Program &rhs)
	{
		return lhs.getName() < rhs.getName();
	}
	bool operator< (const Window &lhs, const Window &rhs)
	{
		return lhs.getTitle() < rhs.getTitle();
	}

	Program::Program(Program &&rhs) :name(std::move(rhs.name)), views(std::move(rhs.views)) {}
	Program &Program::operator=(Program &&rhs) { name = std::move(rhs.name); views = std::move(rhs.views); return *this; }
	Program::Program(const wstring &rhs) :name(rhs), views() {}
	Program::~Program() {}

	shared_ptr<Window> Program::findView(const wstring &rhs)
	{
		for (auto v : views)
		{
			if (v->getTitle() == rhs)
				return v;
		}
		return nullptr;
	}
	Program &Program::addView(const wstring &rhs) 
	{ 
		views.emplace(shared_ptr<Window>(new Window(rhs, this))); 
		return *this;
	}
	wstring Program::getName() const 
	{ 
		return name; 
	}

	Window::Window(Window &&rhs) :title(std::move(rhs.title)), prog(std::move(rhs.prog)), durations(std::move(rhs.durations)) {}
	Window &Window::operator=(Window &&rhs) { title = std::move(rhs.title); prog = std::move(rhs.prog); durations = std::move(rhs.durations); return *this; }
	Window::Window(const wstring &rhs, const Program *p) :title(rhs), prog(p), durations() {}
	Window::~Window() {};
	wstring Window::getTitle() const 
	{ 
		return title; 
	}
	wstring Window::getProgName() const 
	{ 
		return prog->getName(); 
	}
	Window &Window::addDuration(const SYSTEMTIME &lhs, const SYSTEMTIME &rhs) 
	{ 
		durations.emplace(std::make_pair(lhs, rhs)); 
		return *this; 
	}
	size_t Window::totalTimes() const
	{
		return durations.size();
	}
	size_t Window::totalDuration() const
	{
		size_t ret = 0;
		std::for_each(durations.cbegin(), durations.cend(), [&ret](decltype(*durations.cbegin()) it) { ret += GetDiffSeconds(it.first, it.second); });
		return ret;
	}
	wostream &Window::printAll(wostream &os) const
	{
		using std::endl;
		os << title << endl;
		int time = 0;
		for (auto i : durations)
		{
			int tmp = GetDiffSeconds(i.first, i.second);
			os << i.first << " " << i.second << " " << tmp << "s" << endl;
			time += tmp;
		}
		os << "Window Times  = " << durations.size() << " Window Duration  = " << time << "s" << endl;
		os << "----------------------------------------" << endl;
		return os;
	}
	size_t Program::totalTimes() const
	{
		size_t ret = 0;
		std::for_each(views.cbegin(), views.cend(), [&ret](decltype(*views.cbegin()) it) { ret += it->totalTimes(); });
		return ret;
	}
	size_t Program::totalDuration() const
	{
		size_t ret = 0;
		std::for_each(views.cbegin(), views.cend(), [&ret](decltype(*views.cbegin()) it) { ret += it->totalDuration(); });
		return ret;
	}
	wostream &Program::printAll(wostream &os) const
	{
		using std::endl;
		os << name << endl;
		os << "----------------------------------------" << endl;
		for (auto i : views)
		{
			i->printAll(os);
		}
		os << "Program Times = " << totalTimes() << " Program Duration = " << totalDuration() << "s" << endl;
		os << endl;
		return os;
	}
}
