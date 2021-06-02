#include "Engine.h"
#include "Renderer.h"
#include "Chip8.h"

int main(int argc, char** argv) {

	int width = 64, height = 32, scale = 15;
	Engine engine("Hello World!", width, height, scale);

	int increase = (0xFFFFFFFF - 0x000000FF) / (width * height);
	int color = 0x000000FF;

	const int tps = 1;
	float usPerTick = 1000000 / tps;
	sf::Clock clock;

	Chip8 chip8;
	engine.setChip8(&chip8);

	chip8.init();
	chip8.loadRom("ROMS/pong.rom");
	
	while (engine.isRunning()) {

		if (clock.getElapsedTime().asMicroseconds() >= usPerTick) {
			clock.restart();

			engine.handleEvents();

			chip8.emulateCycle();

			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) {
					engine.renderer.setPixel(x, y, chip8.getPixel(x, y));
				}
			}

			engine.displayPixels();
		}
	}


	return 0;

}