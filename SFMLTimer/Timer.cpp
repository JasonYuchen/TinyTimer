#include <SFML/Graphics.hpp>
#include <Windows.h>
#include <string>
#include <iostream>

int main()
{
	sf::RenderWindow window(sf::VideoMode(720, 405), "SFML works!");
	sf::View view(sf::FloatRect(0, 0, 720, 405));
	window.setView(view);
	sf::RectangleShape shape(sf::Vector2f(720,405));
	sf::Font font;
	if (!font.loadFromFile("C:/Windows/Fonts/simhei.ttf"))
	{
		//error
	}
	
	sf::Text text;
	text.setFont(font);
	text.setString("hello");
	text.setCharacterSize(75);
	text.setFillColor(sf::Color::White);
	text.setPosition(sf::Vector2f(160, 90));
	shape.setFillColor(sf::Color(102, 102, 102));
	std::vector<sf::Clock> clock(2);
	std::vector<sf::Time> time(2);
	auto flag = 1;

	while (window.isOpen())
	{
		sf::Event event;
		auto elapsed = time[flag];
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::Resized)
			{
				auto width = event.size.width;
				auto height = event.size.height;
				auto align = width;
				auto factor = 1.0;
				if (width * 9 > height * 16)
				{
					align = height;
					factor = static_cast<double>(align) / 405;
				}
				else
				{
					align = width;
					factor = static_cast<double>(align) / 720;
				}
				shape.setScale(sf::Vector2f(720 * factor, 405 * factor));
				text.setCharacterSize(static_cast<unsigned int>(75 * factor));
				text.setPosition(sf::Vector2f(160 * factor, 90 * factor));
				sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
				window.setView(sf::View(visibleArea));
			}
			if (event.type == sf::Event::MouseButtonReleased)
				if (event.mouseButton.button == sf::Mouse::Left && flag == 1)
				{
					time[flag] += clock[flag].getElapsedTime();
					clock[0].restart();
					clock[1].restart();
					flag = 0;
				}
				else
				{
					time[flag] += clock[flag].getElapsedTime();
					clock[0].restart();
					clock[1].restart();
					flag = 1;
				}
		}
		text.setString(std::to_string((elapsed+clock[flag].getElapsedTime()).asSeconds()));

		window.clear();
		window.draw(shape);
		window.draw(text);
		window.display();
	}

	return 0;
}