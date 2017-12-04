#include<iostream>
#include<iomanip>
#include<algorithm>

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
	void Window::printAll() const
	{
		using std::wcout;
		using std::endl;
		wcout << title << endl;
		int time = 0;
		for (auto i : durations)
		{
			int tmp = GetDiffSeconds(i.first, i.second);
			wcout << i.first << " " << i.second << " " << tmp << "s" << endl;
			time += tmp;
		}
		wcout << "Window Times  = " << durations.size() << " Window Duration  = " << time << "s" << endl;
		wcout << "----------------------------------------" << endl;
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
	void Program::printAll() const
	{
		using std::wcout;
		using std::endl;
		wcout << name << endl;
		wcout << "----------------------------------------" << endl;
		for (auto i : views)
		{
			i->printAll();
		}
		wcout << "Program Times = " << totalTimes() << " Program Duration = " << totalDuration() << "s" << endl;
		wcout << endl;
	}
}
