#include <thread>
#include <condition_variable>
#include <chrono>
#include <mutex>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <array>
using namespace std;

condition_variable cv;
mutex mtx;
sf::Time curTime;
int eventFlag = 0;                       //avoid supurious wake
const size_t TimerInterval = 1000;       //timer frequency = 1/1000ms = 1Hz
const size_t GUIResponseInterval = 50;   //GUI responses per 50ms

bool eventPred()
{
	return eventFlag == 1;
}

void Timer()
{
	int flag = 0;
	sf::Clock clk;
	vector<sf::Time> timer(2);
	unique_lock<mutex> lock(mtx);
	while (true)
	{
		if (cv.wait_for(lock, chrono::milliseconds(TimerInterval), eventPred) == false)   //over 1s
		{
			auto t = clk.getElapsedTime();
			clk.restart();
			timer[flag] += t;
			curTime = timer[flag];
		}
		else                                                                     //event occurs
		{
			auto t = clk.getElapsedTime();
			clk.restart();
			timer[flag] += t;
			++flag;
			flag %= 2;
			curTime = timer[flag];
			eventFlag = 0;
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
	millisec = time.asMilliseconds() % 1000;
	sfTimeToSecond(time, sec);
	sfTimeToMinute(time, min);
	sfTimeToHour(time, hour);
	return{ manio(hour,2), manio(min,2), manio(sec,2), manio(millisec,3) };
}
int main()
{
	const size_t initFontSize = 55;
	const size_t initFontPosX = 50, initFontPosY = 90;
	const size_t initBackGroundSizeX = 720, initBackGroundSizeY = 405;

	sf::RenderWindow window(sf::VideoMode(initBackGroundSizeX, initBackGroundSizeY), "TinyTimer");
	sf::RectangleShape background(sf::Vector2f(initBackGroundSizeX, initBackGroundSizeY));
	background.setFillColor(sf::Color(102, 102, 102));

	sf::Font font;
	if (!font.loadFromFile("C:/Windows/Fonts/simhei.ttf"))
	{
		//error
	}
	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(initFontSize);
	text.setFillColor(sf::Color::White);
	text.setPosition(sf::Vector2f(initFontPosX, initFontPosY));

	thread timerThread(Timer);
	timerThread.detach();
	
	auto time = sfTimeToHMS(curTime);

	while (window.isOpen())
	{
		sf::Event event;
		time = sfTimeToHMS(curTime);
		text.setString(time[0] + L"Ð¡Ê±" + time[1] + L"·ÖÖÓ" + time[2] + L"Ãë" + time[3] + L"ºÁÃë");
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
			else if (event.type == sf::Event::MouseButtonReleased)
			{
				eventFlag = 1;
				cv.notify_one();
			}
			else if (event.type == sf::Event::Resized)
			{
				auto width = event.size.width;
				auto height = event.size.height;
				auto align = width;
				auto factor = 1.0;
				if (width * 9 > height * 16)   //display in 16:9
				{
					align = height;
					factor = static_cast<double>(align) / initBackGroundSizeY;
				}
				else
				{
					align = width;
					factor = static_cast<double>(align) / initBackGroundSizeX;
				}
				background.setScale(sf::Vector2f(initBackGroundSizeX * factor, initBackGroundSizeY * factor));
				text.setCharacterSize(static_cast<unsigned int>(initFontSize * factor));
				text.setPosition(sf::Vector2f(initFontPosX * factor, initFontPosY * factor));

				sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
				window.setView(sf::View(visibleArea));
			}
		}
		this_thread::sleep_for(chrono::milliseconds(GUIResponseInterval));
		window.clear();
		window.draw(background);
		window.draw(text);
		window.display();
	}
	
	return 0;
}