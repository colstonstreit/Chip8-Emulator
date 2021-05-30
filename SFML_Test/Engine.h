#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Network.hpp>
#include <SFML/Audio.hpp>
#include <SFML/System.hpp>

#include "Renderer.h"

#ifdef __DEBUG__
#include <iostream>
#define LOG(x) (std::cout << (x) << std::endl)
#else
#define LOG(x)
#endif

class Engine {

public:
	Engine(const std::string& title, int width, int height, int scale);
	~Engine();

	void handleEvents();
	void displayPixels();

	bool isRunning() {
		return window.isOpen();
	}

	Renderer renderer;
	sf::RenderWindow window;

};

Engine::Engine(const std::string& title, int width, int height, int scale = 1) : 
	renderer(width, height, scale),
	window(sf::VideoMode(width * scale, height * scale), title)
{}

Engine::~Engine() {}

void Engine::handleEvents() {

	sf::Event event;
	while (window.pollEvent(event)) {

		if (event.type == sf::Event::Closed)
			window.close();

	}
}

void Engine::displayPixels() {
	sf::Image image;
	sf::Texture texture;
	sf::Sprite sprite;

	int width = renderer.getWidth(), height = renderer.getHeight(), scale = renderer.getScale();

	image.create(width * scale, height * scale, renderer.getPixels());
	texture.loadFromImage(image);
	sprite.setTexture(texture);

	window.draw(sprite);
	window.display();
}

