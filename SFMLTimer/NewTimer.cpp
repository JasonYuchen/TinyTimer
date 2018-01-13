#include <thread>
#include <condition_variable>
#include <chrono>
#include <mutex>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
using namespace std;

condition_variable cv;
mutex mtx;
sf::Time curTime;
int eventFlag = 0;  //supurious wake

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
		if (cv.wait_for(lock, chrono::milliseconds(1000), eventPred) == false)   //over 1s
		{
			auto t = clk.getElapsedTime();
			clk.restart();
			timer[flag] += t;
			curTime = timer[flag];
			cout << " over 1 s " << endl;
		}
		else                                                                     //event occurs
		{
			auto t = clk.getElapsedTime();
			clk.restart();
			timer[flag] += t;
			++flag;
			flag %= 2;
			curTime = timer[flag];
			cout << " mouse event " << endl;
			eventFlag = 0;
		}
	}
}

int main()
{
	sf::RenderWindow window(sf::VideoMode(720, 405), "TinyTimer");
	sf::RectangleShape shape(sf::Vector2f(720, 405));
	shape.setFillColor(sf::Color(102, 102, 102));

	sf::Font font;
	if (!font.loadFromFile("C:/Windows/Fonts/simhei.ttf"))
	{
		//error
	}
	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(75);
	text.setFillColor(sf::Color::White);
	text.setPosition(sf::Vector2f(160, 90));
	thread timerThread(Timer);
	timerThread.detach();
	while (window.isOpen())
	{
		sf::Event event;
		text.setString(std::to_string(curTime.asSeconds()));
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::MouseButtonReleased)
			{
				eventFlag = 1;
				cv.notify_one();
			}
		}
		this_thread::sleep_for(chrono::milliseconds(50));
		window.clear();
		window.draw(shape);
		window.draw(text);
		window.display();
	}

	return 0;
}