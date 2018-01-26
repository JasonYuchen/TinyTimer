/**************************************************************
*  Filename:    TimerClass.h
*  Copyright:   All rights reserved
*
*  @author:     Jason
*  @version     v0.9
**************************************************************/
#pragma once
#include "MessagePassing.h"
#include <SFML\Graphics.hpp>
#include <vector>
#include <string>
#include <array>

enum class pos { OUT, SWITCH, RESET };

struct period
{
	sf::Time curTime;
	period(const sf::Time &curTime_) :curTime(curTime_) {}
};
struct highlight
{
	pos highlightArea;
	highlight(pos area) :highlightArea(area) {}
};
struct unhighlight
{};
struct switchevent
{};
struct switchtimer
{
	sf::Time curTime;
	switchtimer(const sf::Time &curTime_) :curTime(curTime_) {}
};
struct resetevent
{};
struct resettimer
{
	sf::Time curTime;
	resettimer(const sf::Time &curTime_) :curTime(curTime_) {}
};
struct resize
{
	sf::Vector2u size;
	resize(const sf::Vector2u &size_) :size(size_) {}
};

class timer
{
private:
	messaging::receiver incoming;
	messaging::sender to_render;
	sf::Clock local_clock;
	std::vector<sf::Time> local_timer{ 2 };
	int flag;
public:
	timer(messaging::sender to_render_) :to_render(to_render_), flag(0) {}

	void done()
	{
		get_sender().send(messaging::close_queue());
	}

	messaging::sender get_sender()
	{
		return incoming;
	}

	void run()
	{
		try
		{
			for (;;)
			{
				incoming.wait(std::chrono::milliseconds(1000))
					.handle<messaging::timed_out>(
						[&](const messaging::timed_out &msg)
				{
					auto t = local_clock.getElapsedTime();
					local_clock.restart();
					local_timer[flag] += t;
					to_render.send(period(local_timer[flag]));
				}
						)
					.handle<switchevent>(
						[&](const switchevent &msg)
				{
					auto t = local_clock.getElapsedTime();
					local_clock.restart();
					local_timer[flag] += t;
					++flag;
					flag %= 2;
					to_render.send(switchtimer(local_timer[flag]));
				}
						)
					.handle<resetevent>(
						[&](const resetevent &msg)
				{
					local_timer[0] = sf::Time();
					local_timer[1] = sf::Time();
					to_render.send(resettimer(local_timer[flag]));
				}
				);
			}
		}
		catch (messaging::close_queue&)
		{
		}
	}
};

class render
{
	friend std::array<std::wstring, 4> sfTimeToHMS(const sf::Time &time);
private:
	messaging::receiver incoming;
	sf::RenderWindow &window;
	sf::RectangleShape backgroundArea;
	sf::RectangleShape resetArea;
	sf::RectangleShape switchArea;
	sf::Font font;
	std::vector<sf::Text> text{ 7 };      //L"小时", L"分钟", L"秒", L"毫秒", L"摸"/L"鸡", L"鱼"/L"血", L"RESET"
	std::vector<sf::Text> timetext{ 4 };  //hour, minute, second, millisecond
public:
	render(sf::RenderWindow &window_) :window(window_)
	{
		initialize();
	}

	void initialize();

	void done()
	{
		get_sender().send(messaging::close_queue());
	}

	messaging::sender get_sender()
	{
		return incoming;
	}

	void run()
	{
		try
		{
			setTime(sf::Time());
			displayall();
			for (;;)
			{
				incoming.wait()
					.handle<period>(
						[&](const period &msg)
				{
					setTime(msg.curTime);
					//draw all things per 1s(timed_out)
				}
						)
					.handle<highlight>(
						[&](const highlight &msg)
				{
					switch (msg.highlightArea)
					{
					case pos::SWITCH:
						switchArea.setFillColor(sf::Color(64, 64, 64));
						resetArea.setFillColor(sf::Color(64, 64, 64)); 
						break;
					case pos::RESET:
						resetArea.setFillColor(sf::Color(169, 169, 169)); 
						switchArea.setFillColor(sf::Color(102, 102, 102));
						break;
					}
					//highlight specific area and draw all
				}
						)
					.handle<unhighlight>(
						[&](const unhighlight &msg)
				{
					switchArea.setFillColor(sf::Color(102, 102, 102));
					resetArea.setFillColor(sf::Color(64, 64, 64));
					//unhighlight specific area and draw all
				}
						)
					.handle<switchtimer>(
						[&](const switchtimer &msg)
				{
					switchText();
					setTime(msg.curTime);
					//ji xue <-> mo yu and draw all
				}
						)
					.handle<resettimer>(
						[&](const resettimer &msg)
				{
					setTime(msg.curTime);
				}
						)
					.handle<resize>(
						[&](const resize &msg)
				{
					window.setSize(msg.size);
				}
				);
				displayall();
			}
		}
		catch (messaging::close_queue&)
		{
		}
	}

	void displayall()
	{
		window.clear();
		window.draw(backgroundArea);
		window.draw(switchArea);
		window.draw(resetArea);
		std::for_each(timetext.begin(), timetext.end(), [&](sf::Text &rhs) {window.draw(rhs); });
		std::for_each(text.begin(), text.end(), [&](sf::Text &rhs) {window.draw(rhs); });
		window.display();
	}

	void setTime(const sf::Time &t)
	{
		auto time = sfTimeToHMS(t);
		auto i = 0;
		std::for_each(timetext.begin(), timetext.end(), [&](sf::Text &rhs) {
			rhs.setString(time[i]);
			++i;
		});
	}

	void switchText()
	{
		if (text[4].getString() == L"摸")
		{
			text[4].setString(L"鸡");
			text[5].setString(L"血");
		}
		else
		{
			text[4].setString(L"摸");
			text[5].setString(L"鱼");
		}
	}
};