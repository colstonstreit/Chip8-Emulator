#include "Engine.h"


Engine::Engine(int width, int height, std::string title) : width(width), height(height), title(title) {
	this->window = new sf::RenderWindow(sf::VideoMode(width, height), title);
	this->init();
}

Engine::~Engine() {
	if (window)
		delete window;
}

void Engine::run() {

	sf::Time lastTime = sf::microseconds(0);
	double usPerTick = 1000000 / targetTPS;
	double delta = 0;
	sf::Time totalElapsedTime = sf::seconds(0);

	while (window->isOpen()) {
		sf::Event event;
		while (window->pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window->close();
			else
				handleEvent(event);
		}

		sf::Time timeDiff = clock.getElapsedTime() - lastTime;
		delta += timeDiff.asMicroseconds() / usPerTick;
		lastTime += timeDiff;
		totalElapsedTime += timeDiff;

		if (delta >= 1) {
			delta--;
			update(totalElapsedTime.asSeconds());
			totalElapsedTime = sf::microseconds(0);
			tps++;
		}

		window->clear(sf::Color::Black);
		render();
		window->display();
		fps++;

		if (secondClock.getElapsedTime().asMilliseconds() >= 1000.0) {
			window->setTitle(title + " | TPS, FPS: " + std::to_string(tps) + ", " + std::to_string(fps));
			tps = 0;
			fps = 0;
			secondClock.restart();
		}
	}

}

void Engine::init() {
	shape.setRadius(100.f);
	shape.setFillColor(sf::Color::Red);
}

void Engine::update(float dt) {
	float speed = 100.f;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		shape.move(sf::Vector2f(speed * dt, 0.f));
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		shape.move(sf::Vector2f(-speed * dt, 0.f));
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		shape.move(sf::Vector2f(0.f, -speed * dt));
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		shape.move(sf::Vector2f(0.f, speed * dt));
}

void Engine::render() {
	window->draw(shape);
}

void Engine::handleEvent(sf::Event& event) {

}

