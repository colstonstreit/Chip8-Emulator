#pragma once

#include <SFML/Window.hpp>

class Renderer {

public:
	Renderer(int width, int height, int scale);
	~Renderer();

	sf::Uint32 getPixel(int x, int y);
	void setPixel(int x, int y, sf::Uint32 color);
	void clearDisplay(sf::Uint32 color);
	void fillRect(int x1, int y1, int width, int height, sf::Uint32 color);
	inline int getWidth() { return width; }
	inline int getHeight() { return height; }
	inline int getScale() { return scale; }

	sf::Uint8* getPixels() {
		return byteArray;
	}

private:
	int width = 0, height = 0, scale = 1;
	sf::Uint8* byteArray = nullptr;

	bool isValidCoordinate(int x, int y) {
		return !(x >= width || x < 0 || y >= height || y < 0);
	}

};

Renderer::Renderer(int width, int height, int scale = 1) : width(width), height(height), scale(scale) {
	byteArray = new sf::Uint8[width * height * scale * scale * 4];
	memset(byteArray, 0, width * height * scale * scale * 4);
}

Renderer::~Renderer() {
	delete[] byteArray;
}

sf::Uint32 Renderer::getPixel(int x, int y) {
	int index = x * scale + y * width * scale * scale;
	return (byteArray[index + 0] << 24) | (byteArray[index + 1] << 16) | (byteArray[index + 2] << 8) | byteArray[index + 3];
}

void Renderer::setPixel(int x, int y, sf::Uint32 color) {

	if (isValidCoordinate(x, y)) {

		int effectiveWidth = width * scale;

		for (int i = 0; i < scale; i++) {
			for (int j = 0; j < scale; j++) {
				int index = (x * scale + y * effectiveWidth * scale + i * effectiveWidth + j) * 4;
				byteArray[index + 0] = (color & 0xFF000000) >> 24;
				byteArray[index + 1] = (color & 0x00FF0000) >> 16;
				byteArray[index + 2] = (color & 0x0000FF00) >> 8;
				byteArray[index + 3] = (color & 0x000000FF);
			}
		}

	}
}

void Renderer::clearDisplay(sf::Uint32 color = 0x000000FF) {
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			setPixel(x, y, color);
		}
	}
}

void Renderer::fillRect(int x1, int y1, int width, int height, sf::Uint32 color) {
	for (int y = y1; y < y1 + height; y++) {
		for (int x = x1; x < x1 + width; x++) {
			setPixel(x, y, color);
		}
	}
}
