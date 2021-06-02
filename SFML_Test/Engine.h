#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Network.hpp>
#include <SFML/Audio.hpp>
#include <SFML/System.hpp>

#include "Renderer.h"
#include "Chip8.h"

class Engine {

public:
	Engine(const std::string& title, int width, int height, int scale);
	~Engine();

	void handleEvents();
	void displayPixels();

	void setChip8(Chip8* chip8) {
		this->chip8 = chip8;
	}

	bool isRunning() {
		return window.isOpen();
	}

	bool isActive() {
		return window.hasFocus();
	}

	Renderer renderer;
	sf::RenderWindow window;
	Chip8* chip8;

private:
	sf::Uint8 getKeyCode(sf::Keyboard::Key key);

};

Engine::Engine(const std::string& title, int width, int height, int scale = 1) :
	renderer(width, height, scale),
	window(sf::VideoMode(width* scale, height* scale), title, sf::Style::Close),
	chip8(nullptr)
{}

Engine::~Engine() {}

void Engine::handleEvents() {

	sf::Event event;
	while (window.pollEvent(event)) {

		if (event.type == sf::Event::Closed)
			window.close();

		if (event.type == sf::Event::KeyPressed) {
			chip8->setKey(getKeyCode(event.key.code), true);
		}

		if (event.type == sf::Event::KeyReleased) {
			chip8->setKey(getKeyCode(event.key.code), false);
			if (event.key.code == sf::Keyboard::Key::P) {
				chip8->printStatus();
			}
		}

	}
}

sf::Uint8 Engine::getKeyCode(sf::Keyboard::Key key) {

	switch (key) {
	case sf::Keyboard::Key::Num1:
		return 0x1;
	case sf::Keyboard::Key::Num2:
		return 0x2;
	case sf::Keyboard::Key::Num3:
		return 0x3;
	case sf::Keyboard::Key::Num4:
		return 0xC;
	case sf::Keyboard::Key::Q:
		return 0x4;
	case sf::Keyboard::Key::W:
		return 0x5;
	case sf::Keyboard::Key::E:
		return 0x6;
	case sf::Keyboard::Key::R:
		return 0xD;
	case sf::Keyboard::Key::A:
		return 0x7;
	case sf::Keyboard::Key::S:
		return 0x8;
	case sf::Keyboard::Key::D:
		return 0x9;
	case sf::Keyboard::Key::F:
		return 0xE;
	case sf::Keyboard::Key::Z:
		return 0xA;
	case sf::Keyboard::Key::X:
		return 0x0;
	case sf::Keyboard::Key::C:
		return 0xB;
	case sf::Keyboard::Key::V:
		return 0xF;
	default:
		return -1;
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

