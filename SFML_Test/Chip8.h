#pragma once

#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <fstream>
#include <chrono>
#include <random>
#include <vector>

class Chip8 {

public:

	const int START_ADDRESS = 0x200;
	const int FONTSET_START_ADDRESS = 0x50;
	const int FONT_SIZE = 80;

	Chip8();
	void init();
	void setKey(sf::Uint8 key, bool active);
	void loadRom(const char* file);
	void emulateCycle();
	sf::Uint32 getPixel(int x, int y);
	void printStatus();
	std::vector<sf::Vector2u>& getChangedPixels() { return changedPixels; }

private:
	sf::Uint8 RAM[4096];
	sf::Uint8 registers[16];
	sf::Uint16 index;

	sf::Uint8 delayTimer;
	sf::Uint8 soundTimer;
	sf::Clock sixtyHzCounter;

	sf::Uint16 pc;
	sf::Uint8 sp;
	sf::Uint16 stack[16];

	bool keypad[16];
	sf::Uint8 graphics[32][64];

	sf::Uint16 opcode;

	std::default_random_engine randomGen;
	std::uniform_int_distribution<unsigned int> randomByte;

	sf::Sound tone;
	sf::SoundBuffer soundBuffer;
	bool tonePlaying = false;

	std::vector<sf::Vector2u> changedPixels;
	void setPixel(int x, int y, sf::Uint8 value);

	// Operations
	void decodeInstruction(void(Chip8::* &operation)());

	void OP_00E0(); void OP_00EE(); void OP_1nnn(); void OP_2nnn(); 
	void OP_3xkk(); void OP_4xkk(); void OP_5xy0(); void OP_6xkk();
	void OP_7xkk(); void OP_8xy0(); void OP_8xy1(); void OP_8xy2();
	void OP_8xy3(); void OP_8xy4(); void OP_8xy5(); void OP_8xy6();
	void OP_8xy7(); void OP_8xyE(); void OP_9xy0(); void OP_Annn();
	void OP_Bnnn(); void OP_Cxkk(); void OP_Dxyn(); void OP_Ex9E();
	void OP_ExA1(); void OP_Fx07(); void OP_Fx0A(); void OP_Fx15();
	void OP_Fx18(); void OP_Fx1E(); void OP_Fx29(); void OP_Fx33();
	void OP_Fx55(); void OP_Fx65(); void OP_NULL();
};
