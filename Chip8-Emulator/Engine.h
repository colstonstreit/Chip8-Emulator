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

