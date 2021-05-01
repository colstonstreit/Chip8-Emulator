#ifndef GUARD_ENGINE_H
#define GUARD_ENGINE_H

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Network.hpp>
#include <SFML/Audio.hpp>
#include <SFML/System.hpp>

#ifdef __DEBUG__
#include <iostream>
#define LOG(x) (std::cout << (x) << std::endl)
#else
#define LOG(x)
#endif


class Engine {

public:
	sf::RenderWindow* window = nullptr;
	const float targetTPS = 60.0f;

private:
	int fps = 0;
	int tps = 0;
	int width, height;
	std::string title;

	sf::Clock clock;
	sf::Clock secondClock;
	sf::CircleShape shape;

public:
	Engine(int width, int height, std::string title);
	~Engine();
	void run();

	int getFPS() const {
		return fps;
	}
	int getTPS() const {
		return tps;
	}

private:
	void init();
	void update(float dt);
	void render();
	void handleEvent(sf::Event& event);

};

#endif // ENGINE_H

