#include "Engine.h"
#include "Renderer.h"
#include "Chip8.h"

int main(int argc, char** argv) {

	int width = 64, height = 32, scale = 20;
	Engine engine("Chip-8 Emulator", width, height, scale);

	Chip8 chip8;
	engine.setChip8(&chip8);

	chip8.init();
	chip8.loadRom("ROMS/invaders.rom");
	
	while (engine.isRunning()) {

			engine.handleEvents();

			if (engine.isActive()) {
				chip8.emulateCycle();
			}

			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) {
					engine.renderer.setPixel(x, y, chip8.getPixel(x, y));
				}
			}

			engine.displayPixels();
	}

	return 0;

}