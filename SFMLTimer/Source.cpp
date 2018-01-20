/**************************************************************
*  Filename:    NewTimer.cpp
*  Copyright:   All rights reserved
*
*  @author:     Jason
*  @version     v0.6
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

std::condition_variable Switch_Reset_cv, Render_cv;
std::mutex timeData, windowData;
sf::Time currentTime;
enum class SwitchResetStatus { NONE, SWITCH, RESET };
SwitchResetStatus mouseClick = SwitchResetStatus::NONE;                 //avoid supurious wake
SwitchResetStatus mousePosition = SwitchResetStatus::NONE;
enum class RenderStatus {NONE, TIME, SWITCH, HIGHTLIGHT_SWITCH, HIGHTLIGHT_RESET};
RenderStatus currentRender = RenderStatus::NONE;

std::array<std::wstring, 4> sfTimeToHMS(const sf::Time &time);

void Timer()
{
	int flag = 0;
	sf::Clock clk;
	std::vector<sf::Time> timer(2);
	std::unique_lock<std::mutex> lock(timeData);
	while (true)
	{
		if (Switch_Reset_cv.wait_for(lock, std::chrono::milliseconds(1000), [&]() {return mouseClick != SwitchResetStatus::NONE; }) == false)  // over 1s
		{
			auto t = clk.getElapsedTime();
			clk.restart();
			timer[flag] += t;
			currentTime = timer[flag];
			currentRender = RenderStatus::TIME;
		}
		else                                                                              // event occurs
		{
			if (mouseClick == SwitchResetStatus::SWITCH)
			{
				auto t = clk.getElapsedTime();
				clk.restart();
				timer[flag] += t;
				++flag;
				flag %= 2;
				currentTime = timer[flag];
				currentRender = RenderStatus::SWITCH;
			}
			else if (mouseClick == SwitchResetStatus::RESET)
			{
				timer[0] = sf::Time();
				timer[1] = sf::Time();
				currentTime = timer[flag];
				currentRender = RenderStatus::TIME;
			}
			mouseClick = SwitchResetStatus::NONE;
		}
		//lock.unlock();

		Render_cv.notify_one();
	}
}

void Render(sf::RenderWindow &window)
{
	const size_t initFontSize = 55;
	const size_t initFontPosX = 50, initFontPosY = 90;
	const size_t initBackGroundSizeX = 800, initBackGroundSizeY = 500;
	const size_t resetSizeX = 100, resetSizeY = initBackGroundSizeY;
	const size_t switchAreaSizeX = 500, switchAreaSizeY = initBackGroundSizeY;

	sf::RectangleShape background(sf::Vector2f(initBackGroundSizeX, initBackGroundSizeY));
	background.setFillColor(sf::Color(102, 102, 102));

	sf::RectangleShape reset(sf::Vector2f(100, 500));
	sf::RectangleShape switchArea(sf::Vector2f(500, 500));

	reset.setFillColor(sf::Color(64, 64, 64));
	reset.setPosition(sf::Vector2f(initBackGroundSizeX - resetSizeX, 0));

	switchArea.setFillColor(sf::Color(64, 64, 64));
	switchArea.setPosition(sf::Vector2f(100, 0));

	std::vector<std::pair<std::wstring, std::tuple<unsigned int, float, float, float, sf::Color>>> texts = {    // tuple<fontsize, posX, posY, rot, color>
		{ L"Ð¡Ê±",{ 120, 330,  70,  0,sf::Color::White } },
		{ L"·ÖÖÓ",{ 75, 420, 220,  0,sf::Color::White } },
		{ L"Ãë",{ 160, 410, 300,  0,sf::Color::White } },
		{ L"ºÁÃë",{ 40, 260, 200,  0,sf::Color::White } },
		{ L"Ãþ",{ 120,  80,  20,  0,sf::Color::White } },
		{ L"Óã",{ 120,  80, 133,  0,sf::Color::White } },
		{ L"RESET",{ 150, 640, 440,270,sf::Color::White } }
	};

	sf::Font font;
	if (!font.loadFromFile("C:/Windows/Fonts/simhei.ttf"))
	{
		//error
	}
	std::vector<sf::Text> text(7);
	std::vector<sf::Text> timetext(4);

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

	auto time = sfTimeToHMS(currentTime);
	std::unique_lock<std::mutex> lock(timeData);
	while (true)
	{
		Render_cv.wait(lock, [&]() {return currentRender != RenderStatus::NONE; });
		time = sfTimeToHMS(currentTime);
		// update time texts
		i = 0;
		std::for_each(timetext.begin(), timetext.end(), [&](sf::Text &rhs) {
			rhs.setString(time[i]);
			++i;
		});
		//lock.unlock();

		if (currentRender == RenderStatus::SWITCH)
		{
			if (text[4].getString() == L"Ãþ")
			{
				text[4].setString(L"¼¦");
				text[5].setString(L"Ñª");
			}
			else
			{
				text[4].setString(L"Ãþ");
				text[5].setString(L"Óã");
			}
		}
		else if(currentRender == RenderStatus::HIGHTLIGHT_SWITCH)
		{
			switchArea.setFillColor(sf::Color(64, 64, 64));
		}
		else if(currentRender == RenderStatus::HIGHTLIGHT_RESET)
		{
			reset.setFillColor(sf::Color(169, 169, 169));
		}
		else
		{
			switchArea.setFillColor(sf::Color(102, 102, 102));
			reset.setFillColor(sf::Color(64, 64, 64));
		}
		// render
		{
			//std::lock_guard<std::mutex> lock(windowData);
			window.clear();
			window.draw(background);
			window.draw(switchArea);
			window.draw(reset);
			std::for_each(timetext.begin(), timetext.end(), [&](sf::Text &rhs) {window.draw(rhs); });
			std::for_each(text.begin(), text.end(), [&](sf::Text &rhs) {window.draw(rhs); });
			window.display();
		}
	}
}

int main()
{
	const size_t initBackGroundSizeX = 800, initBackGroundSizeY = 500;
	sf::RenderWindow window(sf::VideoMode(initBackGroundSizeX, initBackGroundSizeY), "TinyTimer");
	window.setSize(sf::Vector2u(initBackGroundSizeX / 2, initBackGroundSizeY / 2));
	window.setActive(false);
	std::thread timerThread(Timer);
	timerThread.detach();
	std::thread renderThread(Render, std::ref(window));
	renderThread.detach();
	auto lastMousePosition = mousePosition;
	while (window.isOpen())
	{
		// get cursor position
		sf::Vector2f mouse{ sf::Mouse::getPosition(window) };

		// wheather in switchArea or reset
		sf::Vector2f windowSize{ window.getSize() };
		if (mouse.x / windowSize.x > 100.0 / 800.0 && mouse.x / windowSize.x < 600.0 / 800.0 && mouse.y > 0 && mouse.y < windowSize.y && lastMousePosition != SwitchResetStatus::SWITCH)
		{
			lastMousePosition = mousePosition;
			mousePosition = SwitchResetStatus::SWITCH;
			currentRender = RenderStatus::HIGHTLIGHT_SWITCH;
			Render_cv.notify_one();
		}
		else if (mouse.x / windowSize.x > 700.0 / 800.0 && mouse.x < windowSize.x && mouse.y > 0 && mouse.y < windowSize.y && lastMousePosition != SwitchResetStatus::RESET)
		{
			lastMousePosition = mousePosition;
			mousePosition = SwitchResetStatus::RESET;
			currentRender = RenderStatus::HIGHTLIGHT_RESET;
			Render_cv.notify_one();
		}
		else
		{
			mousePosition = SwitchResetStatus::NONE;
			currentRender = RenderStatus::NONE;
		}
		sf::Event event;
		{
			//std::lock_guard<std::mutex> lock(windowData);
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
				{
					window.close();
				}
				else if (event.type == sf::Event::MouseButtonReleased && mousePosition == SwitchResetStatus::SWITCH)  // switch status
				{
					mouseClick = SwitchResetStatus::SWITCH;
					Switch_Reset_cv.notify_one();
				}
				else if (event.type == sf::Event::MouseButtonReleased && mousePosition == SwitchResetStatus::RESET) // reset time
				{
					mouseClick = SwitchResetStatus::RESET;
					Switch_Reset_cv.notify_one();
				}
				else if (event.type == sf::Event::Resized)
				{
					auto width = event.size.width;
					auto height = event.size.height;
					if (width * 5 > height * 8)   //display in 16:9
					{
						window.setSize(sf::Vector2u(height * 8 / 5, height));
					}
					else
					{
						window.setSize(sf::Vector2u(width, width * 5 / 8));
					}
					Render_cv.notify_one();
				}
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	return 0;
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
std::wstring manio(size_t val, size_t size, char fill = '0')
{
	auto ret = std::to_wstring(val);
	std::wstring prefix(size - ret.size(), fill);
	return prefix + ret;
}
/* normalize sf::time to hour, minute, second, millisecond of type string */
std::array<std::wstring, 4> sfTimeToHMS(const sf::Time &time)
{
	int millisec, sec, min, hour;
	millisec = time.asMilliseconds();
	sfTimeToSecond(time, sec);
	sfTimeToMinute(time, min);
	sfTimeToHour(time, hour);
	return{ manio(hour,2), manio(min % 60 ,2), manio(sec % 60 ,2), manio(millisec % 1000,3) };
}