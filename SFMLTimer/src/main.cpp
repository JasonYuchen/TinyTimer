/**************************************************************
*  Filename:    main.cpp
*  Copyright:   All rights reserved
*
*  @author:     Jason
*  @version     v1.0
**************************************************************/

#include <iostream>
#include <SFML\Graphics.hpp>
#include <thread>
#include <chrono>
#include "TimerClass.h"
#include "MessagePassing.h"

int main()
{
	// Init the window
	const size_t initBackGroundSizeX = 800, initBackGroundSizeY = 500;
	sf::RenderWindow window(sf::VideoMode(initBackGroundSizeX, initBackGroundSizeY), "TinyTimer");
	window.setSize(sf::Vector2u(initBackGroundSizeX / 2, initBackGroundSizeY / 2));

	// Use another thread to render the window
	window.setActive(false);

	render SFMLRender(window);
	timer SFMLTimer(SFMLRender.get_sender());
	messaging::sender event_to_timer(SFMLTimer.get_sender());
	messaging::sender event_to_render(SFMLRender.get_sender());
	std::thread renderThread(&render::run, &SFMLRender);
	std::thread timerThread(&timer::run, &SFMLTimer);

	auto LastPos = pos::OUT;

	// Event loop
	while (window.isOpen())
	{
		// Get cursor position
		sf::Vector2f mouse{ sf::Mouse::getPosition(window) };

		// Wheather in switchArea or reset
		sf::Vector2f windowSize{ window.getSize() };

		if (mouse.x / windowSize.x > 100.0 / 800.0 &&
			mouse.x / windowSize.x < 600.0 / 800.0 &&
			mouse.y > 0 && mouse.y < windowSize.y &&
			LastPos != pos::SWITCH)
		{
			LastPos = pos::SWITCH;
			event_to_render.send(highlight(pos::SWITCH));
		}
		
		if (mouse.x / windowSize.x > 700.0 / 800.0 &&
			mouse.x < windowSize.x && mouse.y > 0 && 
			mouse.y < windowSize.y && 
			LastPos != pos::RESET)
		{
			LastPos = pos::RESET;
			event_to_render.send(highlight(pos::RESET));
		}
		
		if ((mouse.x / windowSize.x < 100.0 / 800.0 ||
			mouse.x / windowSize.x > 600.0 / 800.0 && mouse.x / windowSize.x < 700.0 / 800.0 ||
			mouse.x > windowSize.x ||
			mouse.y < 0 ||
			mouse.y > windowSize.y)
			&&
			LastPos != pos::OUT)
		{
			LastPos = pos::OUT;
			event_to_render.send(unhighlight());
		}

		sf::Event events;
		while (window.pollEvent(events))
		{
			if (events.type == sf::Event::Closed)
			{
				goto TerminateProgram;
			}
			else if (events.type == sf::Event::MouseButtonReleased && LastPos == pos::SWITCH)
			{
				event_to_timer.send(switchevent());
			}
			else if (events.type == sf::Event::MouseButtonReleased && LastPos == pos::RESET)
			{
				event_to_timer.send(resetevent());
			}
			else if (events.type == sf::Event::Resized)
			{
				auto width = events.size.width;
				auto height = events.size.height;
				auto newsize = sf::Vector2u();

				// Make sure the window keep its scale
				if (width * initBackGroundSizeY > height * initBackGroundSizeX)
				{
					newsize = sf::Vector2u(height * initBackGroundSizeX / initBackGroundSizeY, height);
				}
				else
				{
					newsize = sf::Vector2u(width, width * initBackGroundSizeY / initBackGroundSizeX);
				}
				event_to_render.send(resize(newsize));
			}
		}

		// GUI response interval = 75 ms
		std::this_thread::sleep_for(std::chrono::milliseconds(75));
	}

TerminateProgram:
	// Finish threads
	SFMLTimer.done();
	SFMLRender.done();
	timerThread.join();
	renderThread.join();
	return 0;
}