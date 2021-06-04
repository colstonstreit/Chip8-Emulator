#include "Engine.h"
#include "Renderer.h"
#include "Chip8.h"

int main(int argc, char** argv) {

	int width = 64, height = 32, scale = 10;
	Engine engine("Chip-8 Emulator", width, height, scale);

	Chip8 chip8;
	engine.setChip8(&chip8);
	engine.renderer.clearDisplay();
	engine.displayPixels();

	const float targetClocksPerSec = 700.f;
	float targetMicrosecondDelay = 1000000.f / targetClocksPerSec;
	sf::Clock clock;
	sf::Clock fpsClock;
	const float targetFPS = 20.f;

	chip8.init();
	chip8.loadRom("Roms/Breakout.ch8");

	bool changedDisplay = false;
	
	while (engine.isRunning()) {

		engine.handleEvents();

		if (engine.isActive() && clock.getElapsedTime().asMicroseconds() >= targetMicrosecondDelay) {

			clock.restart();
			chip8.emulateCycle();

			std::vector<sf::Vector2u>& pixels = chip8.getChangedPixels();
			if (!pixels.empty()) {
				for (const sf::Vector2u& pixel : pixels) {
					engine.renderer.setPixel(pixel.x, pixel.y, chip8.getPixel(pixel.x, pixel.y));
				}
				pixels.clear();
				changedDisplay = true;
			}
		}
		if (changedDisplay && fpsClock.getElapsedTime().asMicroseconds() >= 1000000.f / targetFPS) {
			engine.displayPixels();
			changedDisplay = false;
		}

	}

	return 0;

}