﻿/**************************************************************
*  Filename:    TimerClass.cpp
*  Copyright:   All rights reserved
*
*  @author:     Jason
*  @version     v0.9
**************************************************************/
#include "TimerClass.h"
#include <string>
#include <array>
#include <tuple>
#include <utility>

using namespace std;

/* convert sf::Time to second, <1000 millisec = 0 sec */
void sfTimeToSecond(const sf::Time &time, int &second)
{
	auto millisec = time.asMilliseconds();
	second = millisec / 1000;
}
/* convert sf::Time to minute, <60 sec = 0 min */
void sfTimeToMinute(const sf::Time &time, int &minute)
{
	int sec;
	sfTimeToSecond(time, sec);
	minute = sec / 60;
}
/* convert sf::Time to hour, <60 min = 0 hour */
void sfTimeToHour(const sf::Time &time, int &hour)
{
	int min;
	sfTimeToMinute(time, min);
	hour = min / 60;
}
wstring manio(size_t val, size_t size, char fill = '0')
{
	auto ret = to_wstring(val);
	wstring prefix(size - ret.size(), fill);
	return prefix + ret;
}
/* normalize sf::time to hour, minute, second, millisecond of type string */
array<wstring, 4> sfTimeToHMS(const sf::Time &time)
{
	int millisec, sec, min, hour;
	millisec = time.asMilliseconds();
	sfTimeToSecond(time, sec);
	sfTimeToMinute(time, min);
	sfTimeToHour(time, hour);
	return{ manio(hour,2), manio(min % 60 ,2), manio(sec % 60 ,2), manio(millisec % 1000,3) };
}

void render::initialize()
{
	std::vector<std::pair<std::wstring, std::tuple<unsigned int, float, float, float, sf::Color>>> text_property = {    // tuple<fontsize, posX, posY, rot, color>
		{ L"小时",{ 120, 330,  70,  0,sf::Color::White } },
		{ L"分钟",{ 75, 420, 220,  0,sf::Color::White } },
		{ L"秒",{ 160, 410, 300,  0,sf::Color::White } },
		{ L"毫秒",{ 40, 260, 200,  0,sf::Color::White } },
		{ L"摸",{ 120,  80,  20,  0,sf::Color::White } },
		{ L"鱼",{ 120,  80, 133,  0,sf::Color::White } },
		{ L"RESET",{ 150, 640, 440,270,sf::Color::White } }
	};
	if (!font.loadFromFile("C:/Windows/Fonts/simhei.ttf"))
	{
		//error
	}
	// init background/switch/reset area
	backgroundArea.setSize(sf::Vector2f(800, 500));
	backgroundArea.setFillColor(sf::Color(102, 102, 102));
	resetArea.setSize(sf::Vector2f(100, 500));
	resetArea.setFillColor(sf::Color(64, 64, 64));
	resetArea.setPosition(sf::Vector2f(700, 0));
	switchArea.setSize(sf::Vector2f(500, 500));
	switchArea.setFillColor(sf::Color(102, 102, 102));
	switchArea.setPosition(sf::Vector2f(100, 0));

	// init texts' font, fontsize, position, rotation, color
	auto i = 0;
	std::for_each(text.begin(), text.end(), [&](sf::Text &rhs) {
		auto p_name = text_property[i].first;
		auto p_value = text_property[i].second;
		rhs.setFont(font);
		rhs.setStyle(sf::Text::Bold);
		rhs.setCharacterSize(std::get<0>(p_value));
		rhs.setPosition(sf::Vector2f(std::get<1>(p_value), std::get<2>(p_value)));
		rhs.setRotation(std::get<3>(p_value));
		rhs.setFillColor(std::get<4>(p_value));
		rhs.setString(p_name);
		++i;
	});
	i = 0;
	std::for_each(timetext.begin(), timetext.end(), [&](sf::Text &rhs) {
		auto p_name = text_property[i].first;
		auto p_value = text_property[i].second;
		rhs.setFont(font);
		rhs.setStyle(sf::Text::Bold);
		rhs.setCharacterSize(std::get<0>(p_value));
		rhs.setPosition(sf::Vector2f(std::get<1>(p_value) - std::get<0>(p_value), std::get<2>(p_value)));
		rhs.setRotation(std::get<3>(p_value));
		rhs.setFillColor(std::get<4>(p_value));
		++i;
	});
	timetext[3].setPosition(sf::Vector2f(std::get<1>(text_property[3].second) - 1.5 * std::get<0>(text_property[3].second), std::get<2>(text_property[3].second)));          //ms
	timetext[2].setPosition(sf::Vector2f(std::get<1>(text_property[2].second) - 2.0 * std::get<0>(text_property[2].second), std::get<2>(text_property[2].second) - 100));    //s
	timetext[2].setCharacterSize(1.7 * std::get<0>(text_property[2].second));
}
