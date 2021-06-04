#pragma once

#include <SFML/Window.hpp>

class Renderer {

public:
	Renderer(int width, int height, int scale);
	~Renderer();

	sf::Uint32 getPixel(int x, int y);
	void setPixel(int x, int y, sf::Uint32 color);
	void clearDisplay(sf::Uint32 color = 0x000000FF);
	void fillRect(int x1, int y1, int width, int height, sf::Uint32 color);
	inline int getWidth() { return width; }
	inline int getHeight() { return height; }
	inline int getScale() { return scale; }
	inline sf::Uint8* getPixels() { return byteArray; }

private:
	int width = 0, height = 0, scale = 1;
	sf::Uint8* byteArray = nullptr;

	bool isValidCoordinate(int x, int y) {
		return !(x >= width || x < 0 || y >= height || y < 0);
	}

};