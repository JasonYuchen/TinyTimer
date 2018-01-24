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
eventflag mousePosition = NONE;
const size_t TimerInterval = 1000;       //timer frequency = 1/1000ms = 1Hz
const size_t GUIResponseInterval = 75;   //GUI responses per 50ms

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
	const size_t initBackGroundSizeX = 800, initBackGroundSizeY = 500;
	const size_t resetSizeX = 100, resetSizeY = initBackGroundSizeY;
	const size_t switchAreaSizeX = 500, switchAreaSizeY = initBackGroundSizeY;

	sf::RenderWindow window(sf::VideoMode(initBackGroundSizeX, initBackGroundSizeY), "TinyTimer");
	window.setSize(sf::Vector2u(initBackGroundSizeX/2, initBackGroundSizeY/2));

	sf::RectangleShape background(sf::Vector2f(initBackGroundSizeX, initBackGroundSizeY));
	background.setFillColor(sf::Color(102, 102, 102));

	sf::RectangleShape reset(sf::Vector2f(resetSizeX, resetSizeY));
	reset.setFillColor(sf::Color(64, 64, 64));
	reset.setPosition(sf::Vector2f(initBackGroundSizeX-resetSizeX,0));

	sf::RectangleShape switchArea(sf::Vector2f(switchAreaSizeX, switchAreaSizeY));
	switchArea.setFillColor(sf::Color(64, 64, 64));
	switchArea.setPosition(sf::Vector2f(100, 0));

	vector<pair<wstring, tuple<unsigned int, float, float, float, sf::Color>>> texts = {    // tuple<fontsize, posX, posY, rot, color>
		{ L"小时",	{ 120, 330,  70,  0,sf::Color::White } },
		{ L"分钟",	{  75, 420, 220,  0,sf::Color::White } },
		{ L"秒",		{ 160, 410, 300,  0,sf::Color::White } },
		{ L"毫秒",	{  40, 260, 200,  0,sf::Color::White } },
		{ L"摸",		{ 120,  80,  20,  0,sf::Color::White } },
		{ L"鱼",		{ 120,  80, 133,  0,sf::Color::White } },
		{ L"RESET", { 150, 640, 440,270,sf::Color::White } }
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
	std::for_each(text.begin(), text.end(), [&](sf::Text &rhs) {
		rhs.setFont(font); 
		rhs.setStyle(sf::Text::Bold);
		rhs.setCharacterSize(std::get<0>(texts[i].second));
		rhs.setPosition(sf::Vector2f(std::get<1>(texts[i].second), std::get<2>(texts[i].second)));
		rhs.setRotation(std::get<3>(texts[i].second));
		rhs.setFillColor(std::get<4>(texts[i].second));
		rhs.setString(texts[i].first);
		++i;
	});
	i = 0;
	std::for_each(timetext.begin(), timetext.end(), [&](sf::Text &rhs) {
		rhs.setFont(font);
		rhs.setStyle(sf::Text::Bold);
		rhs.setCharacterSize(std::get<0>(texts[i].second));
		rhs.setPosition(sf::Vector2f(std::get<1>(texts[i].second) - std::get<0>(texts[i].second), std::get<2>(texts[i].second)));
		rhs.setRotation(std::get<3>(texts[i].second));
		rhs.setFillColor(std::get<4>(texts[i].second));
		++i;
	});
	timetext[3].setPosition(sf::Vector2f(std::get<1>(texts[3].second) - 1.5 * std::get<0>(texts[3].second), std::get<2>(texts[3].second)));          //ms
	timetext[2].setPosition(sf::Vector2f(std::get<1>(texts[2].second) - 2.0 * std::get<0>(texts[2].second), std::get<2>(texts[2].second) - 100));    //s
	timetext[2].setCharacterSize(1.7 * std::get<0>(texts[2].second));

	thread timerThread(Timer);
	timerThread.detach();
	auto time = sfTimeToHMS(curTime);

	while (window.isOpen())
	{
		sf::Event event;
		time = sfTimeToHMS(curTime);

		// update time texts
		i = 0;
		std::for_each(timetext.begin(), timetext.end(), [&](sf::Text &rhs) {
			rhs.setString(time[i]);
			++i;
		});

		// get cursor position
		sf::Vector2f mouse{ sf::Mouse::getPosition(window) };

		// wheather in switchArea or reset
		sf::Vector2f windowSize{ window.getSize() };
		if (mouse.x/windowSize.x > 100.0/800.0 && mouse.x / windowSize.x < 600.0/800.0 && mouse.y > 0 && mouse.y < windowSize.y)
		{
			switchArea.setFillColor(sf::Color(64, 64, 64));
			mousePosition = SWITCH;
		}
		else if (mouse.x / windowSize.x > 700.0 / 800.0 && mouse.x < windowSize.x && mouse.y > 0 && mouse.y < windowSize.y)
		{
			reset.setFillColor(sf::Color(169, 169, 169));
			mousePosition = RESET;
		}
		else
		{
			reset.setFillColor(sf::Color(64, 64, 64));
			switchArea.setFillColor(sf::Color(102, 102, 102));
			mousePosition = NONE;
		}

		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
			else if (event.type == sf::Event::MouseButtonReleased && mousePosition == SWITCH)  // switch status
			{
				status = SWITCH;
				cv.notify_one();
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
			else if (event.type == sf::Event::MouseButtonReleased && mousePosition == RESET) // reset time
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
				if (width * 5 > height * 8)   //display in 16:9
				{
					//align = height;
					//factor = static_cast<double>(align) / initBackGroundSizeY;
					window.setSize(sf::Vector2u(height * 8 / 5, height));
				}
				else
				{
					//align = width;
					//factor = static_cast<double>(align) / initBackGroundSizeX;
					window.setSize(sf::Vector2u(width, width * 5 / 8));
				}
				
				//reset.setScale(sf::Vector2f(factor, factor));
				// resize event is not implemented yet
				//text.setCharacterSize(static_cast<unsigned int>(initFontSize * factor));
				//text.setPosition(sf::Vector2f(initFontPosX * factor, initFontPosY * factor));

				//sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
				//window.setView(sf::View(visibleArea));
			}
		}



		window.clear();
		window.draw(background);
		window.draw(switchArea);
		window.draw(reset);
		std::for_each(timetext.begin(), timetext.end(), [&](sf::Text &rhs) {window.draw(rhs); });
		std::for_each(text.begin(), text.end(), [&](sf::Text &rhs) {window.draw(rhs); });
		window.display();


		this_thread::sleep_for(chrono::milliseconds(GUIResponseInterval));
	}
	
	return 0;
}
