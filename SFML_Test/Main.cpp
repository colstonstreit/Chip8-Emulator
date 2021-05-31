#include "Engine.h"
#include "Renderer.h"

int main(int argc, char** argv) {

	int width = 65, height = 63, scale = 20;
	Engine engine("Hello World!", width, height, scale);

	int increase = (0xFFFFFFFF - 0x000000FF) / (width * height);
	int color = 0x000000FF;

	const int tps = 20;
	float usPerTick = 1000000 / tps;
	sf::Clock clock;
	
	while (engine.isRunning()) {

		if (clock.getElapsedTime().asMicroseconds() >= usPerTick) {
			clock.restart();

			engine.handleEvents();

			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) {
					engine.renderer.setPixel(x, y, color);
					color += increase;
					if (color < 0x000000FF) color = 0x000000FF;
				}
			}

			engine.displayPixels();
		}
	}


	return 0;

}