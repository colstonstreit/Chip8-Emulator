#pragma once

#include <SFML/Window.hpp>
#include <iostream>
#include <fstream>
#include <chrono>
#include <random>

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

private:
	sf::Uint8 RAM[4096];
	sf::Uint8 registers[16];
	sf::Uint16 index;

	sf::Uint8 delayTimer;
	sf::Uint8 soundTimer;

	sf::Uint16 pc;
	sf::Uint8 sp;
	sf::Uint16 stack[16];

	bool keypad[16];
	sf::Uint8 graphics[64 * 32];

	sf::Uint16 opcode;

	std::default_random_engine randomGen;
	std::uniform_int_distribution<unsigned int> randomByte;

	// OPS
	void decodeInstruction(void(Chip8::* &operation)());

	void OP_00E0();
	void OP_00EE();
	void OP_1nnn();
	void OP_2nnn();
	void OP_3xkk();
	void OP_4xkk();
	void OP_5xy0();
	void OP_6xkk();
	void OP_7xkk();
	void OP_8xy0();
	void OP_8xy1();
	void OP_8xy2();
	void OP_8xy3();
	void OP_8xy4();
	void OP_8xy5();
	void OP_8xy6();
	void OP_8xy7();
	void OP_8xyE();
	void OP_9xy0();
	void OP_Annn();
	void OP_Bnnn();
	void OP_Cxkk();
	void OP_Dxyn();
	void OP_Ex9E();
	void OP_ExA1();
	void OP_Fx07();
	void OP_Fx0A();
	void OP_Fx15();
	void OP_Fx18();
	void OP_Fx1E();
	void OP_Fx29();
	void OP_Fx33();
	void OP_Fx55();
	void OP_Fx65();
	void OP_NULL();
};

Chip8::Chip8() : randomGen(std::chrono::system_clock::now().time_since_epoch().count()) {
	randomByte = std::uniform_int_distribution<unsigned int>(0, 0xFF);
}

void Chip8::init() {
	memset(RAM, 0, 4096);
	memset(registers, 0, 16);
	memset(stack, 0, 16 * 2);
	memset(keypad, 0, 16);
	memset(graphics, 0, 64 * 32);
	delayTimer = 0;
	soundTimer = 0;
	index = 0;
	pc = START_ADDRESS;
	sp = 0;

	// Fonts
	sf::Uint8 fonts[] = {
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

	memcpy(&RAM[FONTSET_START_ADDRESS], fonts, FONT_SIZE);
}

void Chip8::setKey(sf::Uint8 key, bool active) {
	if (key >= 0 && key < 16)
		keypad[key] = active;
}

void Chip8::loadRom(const char* fileName) {
	
	std::ifstream file(fileName, std::ios::in | std::ios::binary);
	if (!file.is_open()) {
		printf("%s is not a valid file!\n", fileName);
		return;
	}

	file.seekg(0, std::ios::end);
	std::streampos size = file.tellg();
	file.seekg(0, std::ios::beg);
	file.read((char*) &RAM[START_ADDRESS], size);
	file.close();

	printf("Loaded %s successfully!\n", fileName);
}

void Chip8::emulateCycle() {
	
	// Fetch
	opcode = RAM[pc] << 8 | RAM[pc + 1];
	pc += 2;

	printf("Opcode: %x\n", opcode);

	// Decode
	void (Chip8::*operation)() = &Chip8::OP_NULL;
	decodeInstruction(operation);

	// Execute
	(this->*operation)();
}

inline sf::Uint32 Chip8::getPixel(int x, int y) {
	return (graphics[y * 32 + x]) ? 0xFFFFFFFF : 0x00000000;
}

void Chip8::decodeInstruction(void(Chip8::* &operation)()) {
	switch ((opcode & 0xF000) >> 12) {
	case 0x0:
		if ((opcode & 0xFFF0) >> 4 == 0x00E) {
			switch (opcode & 0x000F) {
			case 0x0:
				operation = &Chip8::OP_00E0;
				break;
			case 0xE:
				operation = &Chip8::OP_00EE;
				break;
			}
		}
		break;
	case 0x1:
		operation = &Chip8::OP_1nnn;
		break;
	case 0x2:
		operation = &Chip8::OP_2nnn;
		break;
	case 0x3:
		operation = &Chip8::OP_3xkk;
		break;
	case 0x4:
		operation = &Chip8::OP_4xkk;
		break;
	case 0x5:
		operation = &Chip8::OP_5xy0;
		break;
	case 0x6:
		operation = &Chip8::OP_6xkk;
		break;
	case 0x7:
		operation = &Chip8::OP_7xkk;
		break;
	case 0x8:
		switch (opcode & 0x000F) {
		case 0x0:
			operation = &Chip8::OP_8xy0;
			break;
		case 0x1:
			operation = &Chip8::OP_8xy1;
			break;
		case 0x2:
			operation = &Chip8::OP_8xy2;
			break;
		case 0x3:
			operation = &Chip8::OP_8xy3;
			break;
		case 0x4:
			operation = &Chip8::OP_8xy4;
			break;
		case 0x5:
			operation = &Chip8::OP_8xy5;
			break;
		case 0x6:
			operation = &Chip8::OP_8xy6;
			break;
		case 0x7:
			operation = &Chip8::OP_8xy7;
			break;
		case 0xE:
			operation = &Chip8::OP_8xyE;
			break;
		}
		break;
	case 0x9:
		operation = &Chip8::OP_9xy0;
		break;
	case 0xA:
		operation = &Chip8::OP_Annn;
		break;
	case 0xB:
		operation = &Chip8::OP_Bnnn;
		break;
	case 0xC:
		operation = &Chip8::OP_Cxkk;
		break;
	case 0xD:
		operation = &Chip8::OP_Dxyn;
		break;
	case 0xE:
		switch (opcode & 0x00FF) {
		case 0x9E:
			operation = &Chip8::OP_Ex9E;
			break;
		case 0xA1:
			operation = &Chip8::OP_ExA1;
			break;
		}
		break;
	case 0xF:
		switch (opcode & 0x00FF) {
		case 0x07:
			operation = &Chip8::OP_Fx07;
			break;
		case 0x0A:
			operation = &Chip8::OP_Fx0A;
			break;
		case 0x15:
			operation = &Chip8::OP_Fx15;
			break;
		case 0x18:
			operation = &Chip8::OP_Fx18;
			break;
		case 0x1E:
			operation = &Chip8::OP_Fx1E;
			break;
		case 0x29:
			operation = &Chip8::OP_Fx29;
			break;
		case 0x33:
			operation = &Chip8::OP_Fx33;
			break;
		case 0x55:
			operation = &Chip8::OP_Fx55;
			break;
		case 0x65:
			operation = &Chip8::OP_Fx65;
			break;
		}
		break;
	default:
		operation = &Chip8::OP_NULL;
		break;
	}
}

void Chip8::OP_00E0() {
	// Clear the display
	memset(graphics, 0, sizeof(graphics));
}

void Chip8::OP_00EE() {
	// Return from a subroutine
	sp--;
	pc = stack[sp];
}

void Chip8::OP_1nnn() {
	// Jumps to address nnn
	pc = opcode & 0x0FFF;
}

void Chip8::OP_2nnn() {
	// Calls subroutine located at nnn
	stack[sp] = pc;
	sp++;
	pc = opcode & 0x0FFF;
}

void Chip8::OP_3xkk() {
	// Skips next instruction if Vx == kk.
	sf::Uint8 registerIndex = (opcode & 0x0F00) >> 8;
	sf::Uint8 value = opcode & 0x00FF;
	if (registers[registerIndex] == value) {
		pc += 2;
	}
}

void Chip8::OP_4xkk() {
	// Skips next instruction if Vx != kk.
	sf::Uint8 registerIndex = (opcode & 0x0F00) >> 8;
	sf::Uint8 value = opcode & 0x00FF;
	if (registers[registerIndex] != value) {
		pc += 2;
	}
}

void Chip8::OP_5xy0() {
	// Skips next instruction if Vx == Vy.
	sf::Uint8 reg1 = (opcode & 0x0F00) >> 8;
	sf::Uint8 reg2 = (opcode & 0x00F0) >> 4;
	if (registers[reg1] == registers[reg2]) {
		pc += 2;
	}
}

void Chip8::OP_6xkk() {
	// Set Vx = kk.
	sf::Uint8 registerIndex = (opcode & 0x0F00) >> 8;
	sf::Uint8 value = opcode & 0x00FF;
	registers[registerIndex] = value;
}

void Chip8::OP_7xkk() {
	// Set Vx = Vx + kk.
	sf::Uint8 registerIndex = (opcode & 0x0F00) >> 8;
	sf::Uint8 value = opcode & 0x00FF;
	registers[registerIndex] += value;
}

void Chip8::OP_8xy0() {
	// Set Vx = Vy.
	sf::Uint8 vx = (opcode & 0x0F00) >> 8;
	sf::Uint8 vy = (opcode & 0x00F0) >> 4;
	registers[vx] = registers[vy];
}

void Chip8::OP_8xy1() {
	// Set Vx = Vx OR Vy.
	sf::Uint8 vx = (opcode & 0x0F00) >> 8;
	sf::Uint8 vy = (opcode & 0x00F0) >> 4;
	registers[vx] |= registers[vy];
}

void Chip8::OP_8xy2() {
	// Set Vx = Vx AND Vy.
	sf::Uint8 vx = (opcode & 0x0F00) >> 8;
	sf::Uint8 vy = (opcode & 0x00F0) >> 4;
	registers[vx] &= registers[vy];
}

void Chip8::OP_8xy3() {
	// Set Vx = Vx XOR Vy.
	sf::Uint8 vx = (opcode & 0x0F00) >> 8;
	sf::Uint8 vy = (opcode & 0x00F0) >> 4;
	registers[vx] ^= registers[vy];
}

void Chip8::OP_8xy4() {
	// Set Vx = Vx + Vy, set Vf = carry.
	sf::Uint8 vx = (opcode & 0x0F00) >> 8;
	sf::Uint8 vy = (opcode & 0x00F0) >> 4;
	sf::Uint16 sum = vx + vy;
	registers[vx] = sum & 0xFF;
	registers[0xF] = (sum > 0xFF) ? 1 : 0;
}

void Chip8::OP_8xy5() {
	// Set Vx = Vx - Vy, set Vf = NOT borrow.
	sf::Uint8 vx = (opcode & 0x0F00) >> 8;
	sf::Uint8 vy = (opcode & 0x00F0) >> 4;
	registers[0xF] = (registers[vx] > registers[vy]) ? 1 : 0;
	registers[vx] -= registers[vy];
}

void Chip8::OP_8xy6() {
	// Set Vx = Vx >> 1 and Vf = least significant bit of Vx.
	sf::Uint8 vx = (opcode & 0x0F00) >> 8;
	registers[0xF] = registers[vx] & 0x1;
	registers[vx] >>= 1;
}

void Chip8::OP_8xy7() {
	// Set Vx = Vy - Vx, set Vf = NOT borrow.
	sf::Uint8 vx = (opcode & 0x0F00) >> 8;
	sf::Uint8 vy = (opcode & 0x00F0) >> 4;
	registers[0xF] = (registers[vy] > registers[vx]) ? 1 : 0;
	registers[vx] = registers[vy] - registers[vx];
}

void Chip8::OP_8xyE() {
	// Set Vx = Vx << 1 and Vf = most significant bit of Vx.
	sf::Uint8 vx = (opcode & 0x0F00) >> 8;
	registers[0xF] = (registers[vx] & 0x80) >> 7;
	registers[vx] <<= 1;
}

void Chip8::OP_9xy0() {
	// Skips next instruction if Vx != Vy.
	sf::Uint8 reg1 = (opcode & 0x0F00) >> 8;
	sf::Uint8 reg2 = (opcode & 0x00F0) >> 4;
	if (registers[reg1] != registers[reg2]) {
		pc += 2;
	}
}

void Chip8::OP_Annn() {
	// Set I = nnn.
	index = opcode & 0x0FFF;
}

void Chip8::OP_Bnnn() {
	// Jump to location nnn + V0.
	pc = (opcode & 0x0FFF) + registers[0];
}

void Chip8::OP_Cxkk() {
	// Set vx = random byte AND kk.
	sf::Uint8 vx = (opcode & 0x0F00) >> 8;
	sf::Uint8 byte = opcode & 0x00FF;
	registers[vx] = randomByte(randomGen) & byte;
}

void Chip8::OP_Dxyn() {
	// Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision = if pixels were erased.
	sf::Uint8 startX = registers[(opcode & 0x0F00) >> 8];
	sf::Uint8 startY = registers[(opcode & 0x00F0) >> 4];
	sf::Uint8 numBytes = opcode & 0x000F;

	registers[0xF] = 0;

	for (int i = 0; i < numBytes; i++) {
		sf::Uint8 byte = RAM[index + i];
		for (int j = 0; j < 8; j++) {
			sf::Uint8 bit = (byte >> (7 - j)) & 0x1;
			sf::Uint32 graphicsIndex = ((startY + i) % 32) * 32 + ((startX + j) % 64);
			int xorResult = bit ^ graphics[graphicsIndex];
		}
	}
}

inline void Chip8::OP_Ex9E()
{
}

inline void Chip8::OP_ExA1()
{
}

inline void Chip8::OP_Fx07()
{
}

inline void Chip8::OP_Fx0A()
{
}

inline void Chip8::OP_Fx15()
{
}

inline void Chip8::OP_Fx18()
{
}

inline void Chip8::OP_Fx1E()
{
}

inline void Chip8::OP_Fx29()
{
}

inline void Chip8::OP_Fx33()
{
}

inline void Chip8::OP_Fx55()
{
}

inline void Chip8::OP_Fx65()
{
}

void Chip8::OP_NULL() {}

