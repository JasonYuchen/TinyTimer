/**************************************************************
 *  Filename:    NewTimer.cpp
 *  Copyright:   All rights reserved
 *
 *  @author:     Jason
 *  @version     v0.02
 **************************************************************/

#include <thread>
#include <condition_variable>
#include <chrono>
#include <mutex>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <utility>
#include <tuple>
using namespace std;

/* Caution : changing system time may freeze the application */

condition_variable cv;
mutex mtx;
sf::Time curTime;
enum eventflag { NONE, SWITCH, RESET };
eventflag status = NONE;                 //avoid supurious wake
const size_t TimerInterval = 1000;       //timer frequency = 1/1000ms = 1Hz
const size_t GUIResponseInterval = 50;   //GUI responses per 50ms

bool eventPred()
{
	return status != NONE;
}

void Timer()
{
	int flag = 0;
	sf::Clock clk;
	vector<sf::Time> timer(2);
	unique_lock<mutex> lock(mtx);
	while (true)
	{
		if (cv.wait_for(lock, chrono::milliseconds(TimerInterval), eventPred) == false)   // over 1s
		{
			auto t = clk.getElapsedTime();
			clk.restart();
			timer[flag] += t;
			curTime = timer[flag];
			status = NONE;
		}
		else                                                                              // event occurs
		{
			if (status == SWITCH)
			{
				auto t = clk.getElapsedTime();
				clk.restart();
				timer[flag] += t;
				++flag;
				flag %= 2;
				curTime = timer[flag];
			}
			else if (status == RESET)
			{
				timer[0] = sf::Time();
				timer[1] = sf::Time();
				curTime = timer[flag];
			}
			status = NONE;
		}
	}
}

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
int main()
{
	const size_t initFontSize = 55;
	const size_t initFontPosX = 50, initFontPosY = 90;
	const size_t initBackGroundSizeX = 720, initBackGroundSizeY = 540;

	sf::RenderWindow window(sf::VideoMode(initBackGroundSizeX, initBackGroundSizeY), "TinyTimer");
	sf::RectangleShape background(sf::Vector2f(initBackGroundSizeX, initBackGroundSizeY));
	background.setFillColor(sf::Color(102, 102, 102));

	vector<pair<wstring, tuple<unsigned int, float, float, float, sf::Color>>> texts = {    // tuple<fontsize, posX, posY, rot, color>
		{ L"–° ±",	{ 55,150, 20,  0,sf::Color::White } },
		{ L"∑÷÷”",	{ 55,150, 80,  0,sf::Color::White } },
		{ L"√Î",		{ 55, 80,140,  0,sf::Color::White } },
		{ L"∫¡√Î",	{ 20,100,200,  0,sf::Color::White } },
		{ L"√˛",		{ 55,  0,  0,  0,sf::Color::White } },
		{ L"”„",		{ 55,  0, 50,  0,sf::Color::White } },
		{ L"RESET", { 55,600,200,270,sf::Color::White } }
	};

	sf::Font font;
	if (!font.loadFromFile("C:/Windows/Fonts/simhei.ttf"))
	{
		//error
	}
	vector<sf::Text> text(7);
	vector<sf::Text> timetext(4);

	// init texts' font, fontsize, position, rotation, color
	auto i = 0;
	for_each(text.begin(), text.end(), [&](sf::Text &rhs) {
		rhs.setFont(font); 
		rhs.setCharacterSize(std::get<0>(texts[i].second));
		rhs.setPosition(sf::Vector2f(std::get<1>(texts[i].second), std::get<2>(texts[i].second)));
		rhs.setRotation(std::get<3>(texts[i].second));
		rhs.setFillColor(std::get<4>(texts[i].second));
		rhs.setString(texts[i].first);
		++i;
	});
	i = 0;
	for_each(timetext.begin(), timetext.end(), [&](sf::Text &rhs) {
		rhs.setFont(font);
		rhs.setCharacterSize(std::get<0>(texts[i].second));
		rhs.setPosition(sf::Vector2f(std::get<1>(texts[i].second) - 50, std::get<2>(texts[i].second)));
		rhs.setRotation(std::get<3>(texts[i].second));
		rhs.setFillColor(std::get<4>(texts[i].second));
		++i;
	});

	thread timerThread(Timer);
	timerThread.detach();
	
	auto time = sfTimeToHMS(curTime);

	while (window.isOpen())
	{
		sf::Event event;
		time = sfTimeToHMS(curTime);

		// update time texts
		i = 0;
		for_each(timetext.begin(), timetext.end(), [&](sf::Text &rhs) {
			rhs.setString(time[i]);
			++i;
		});

		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
			else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)  // switch status
			{
				status = SWITCH;
				cv.notify_one();
				if (text[4].getString() == L"√˛")
				{
					text[4].setString(L"º¶");
					text[5].setString(L"—™");
				}
				else
				{
					text[4].setString(L"√˛");
					text[5].setString(L"”„");
				}
			}
			else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Right) // reset time
			{
				status = RESET;
				cv.notify_one();
			}
			else if (event.type == sf::Event::Resized)
			{
				auto width = event.size.width;
				auto height = event.size.height;
				//auto align = width;
				//auto factor = 1.0;
				if (width * 3 > height * 4)   //display in 16:9
				{
					//align = height;
					//factor = static_cast<double>(align) / initBackGroundSizeY;
					window.setSize(sf::Vector2u(height * 4 / 3, height));
				}
				else
				{
					//align = width;
					//factor = static_cast<double>(align) / initBackGroundSizeX;
					window.setSize(sf::Vector2u(width, width * 3 / 4));
				}
				//background.setScale(sf::Vector2f(initBackGroundSizeX * factor, initBackGroundSizeY * factor));
				// resize event is not implemented yet
				//text.setCharacterSize(static_cast<unsigned int>(initFontSize * factor));
				//text.setPosition(sf::Vector2f(initFontPosX * factor, initFontPosY * factor));

				//sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
				//window.setView(sf::View(visibleArea));
			}
		}
		this_thread::sleep_for(chrono::milliseconds(GUIResponseInterval));
		window.clear();
		window.draw(background);
		for_each(timetext.begin(), timetext.end(), [&](sf::Text &rhs) {window.draw(rhs); });
		for_each(text.begin(), text.end(), [&](sf::Text &rhs) {window.draw(rhs); });
		window.display();
	}
	
	return 0;
}
