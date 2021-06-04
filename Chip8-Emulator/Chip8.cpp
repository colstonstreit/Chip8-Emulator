#include "Chip8.h"

Chip8::Chip8() : randomGen(std::chrono::system_clock::now().time_since_epoch().count()) {
	randomByte = std::uniform_int_distribution<unsigned int>(0, 0xFF);

	if (!soundBuffer.loadFromFile("ShortBeep.wav")) {
		printf("Audio file did not load correctly.\n");
	}

	tone.setBuffer(soundBuffer);
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
	if (key >= 0 && key < 16) {
		keypad[key] = active;
	}
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
	file.read((char*)&RAM[START_ADDRESS], size);
	file.close();

	printf("Loaded %s successfully!\n", fileName);
}

void Chip8::emulateCycle() {

	// Fetch
	opcode = RAM[pc] << 8 | RAM[pc + 1];
	pc += 2;

	// Decode
	void (Chip8::*operation)() = &Chip8::OP_NULL;
	decodeInstruction(operation);

	// Execute
	(this->*operation)();
	if (sixtyHzCounter.getElapsedTime().asMilliseconds() >= 1000.f / 60.f) {
		sixtyHzCounter.restart();
		if (delayTimer > 0) delayTimer--;
		if (soundTimer > 0) {
			if (!tonePlaying) {
				tonePlaying = true;
				tone.play();
			}
			soundTimer--;
			if (soundTimer == 0) {
				tonePlaying = false;
			}
		}
	}
}

sf::Uint32 Chip8::getPixel(int x, int y) {
	return (graphics[y][x]) ? 0xFFFFFFFF : 0x000000FF;
}

void Chip8::printStatus() {

	printf("\n=================================== STATUS ===================================\n");

	// Opcode
	printf("Current instruction: %x\n\n", opcode);
	// Registers
	printf("Registers:\n");
	printf("\t");
	for (int i = 0; i <= 0xF; i++) {
		printf("%3x ", i);
	}
	printf("\n\t");
	for (int i = 0; i <= 0xF; i++) {
		printf("%3x ", registers[i]);
	}
	printf("\n\n");

	// Stack
	printf("Stack:\n");
	printf("\t");
	for (int i = 0; i < sp; i++) {
		printf("%4x ", stack[i]);
	}
	printf("\n\n");

	// Keypad
	printf("Keypad:\n");
	printf("\t");
	for (int i = 0; i <= 0xF; i++) {
		printf("%3x ", i);
	}
	printf("\n\t");
	for (int i = 0; i <= 0xF; i++) {
		printf("%3d ", keypad[i]);
	}
	printf("\n\n");

	// Program Counter and Index
	printf("Program Counter: %d\nIndex: %d\n\n", pc, index);

	// Timers
	printf("Delay Timer: %d\nSound Timer: %d\n", delayTimer, soundTimer);

}

void Chip8::setPixel(int x, int y, sf::Uint8 value) {
	graphics[y][x] = value;
	changedPixels.emplace_back(x, y);
}

void Chip8::decodeInstruction(void(Chip8::*& operation)()) {
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
	for (int y = 0; y < 32; y++) {
		for (int x = 0; x < 64; x++) {
			setPixel(x, y, 0);
		}
	}
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
	sf::Uint16 sum = registers[vx] + registers[vy];
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
			if (bit) {
				if (graphics[startY + i][startX + j]) {
					registers[0xF] = 1;
					setPixel(startX + j, startY + i, 0);
				}
				else {
					setPixel(startX + j, startY + i, 1);
				}
			}
		}
	}
}

void Chip8::OP_Ex9E() {
	// Skip next instruction if key with value in Vx is pressed.
	sf::Uint8 vx = (opcode & 0x0F00) >> 8;
	if (keypad[registers[vx]]) {
		pc += 2;
	}
}

void Chip8::OP_ExA1() {
	// Skip next instruction if key with value in Vx is not pressed.
	sf::Uint8 vx = (opcode & 0x0F00) >> 8;
	if (!keypad[registers[vx]]) {
		pc += 2;
	}
}

void Chip8::OP_Fx07() {
	// Set Vx = delay timer value.
	sf::Uint8 vx = (opcode & 0x0F00) >> 8;
	registers[vx] = delayTimer;
}

void Chip8::OP_Fx0A() {
	// Wait for a key press, store the value of the key in Vx.
	sf::Uint8 vx = (opcode & 0x0F00) >> 8;
	for (int i = 0; i <= 0xF; i++) {
		if (keypad[i]) {
			registers[vx] = i;
			return;
		}
	}
	pc -= 2;
}

void Chip8::OP_Fx15() {
	// Set delay timer = Vx.
	sf::Uint8 vx = (opcode & 0x0F00) >> 8;
	delayTimer = registers[vx];
}

void Chip8::OP_Fx18() {
	// Set sound timer = Vx.
	sf::Uint8 vx = (opcode & 0x0F00) >> 8;
	soundTimer = registers[vx];
}

void Chip8::OP_Fx1E() {
	// Set I = I + Vx.
	sf::Uint8 vx = (opcode & 0x0F00) >> 8;
	index += registers[vx];
}

void Chip8::OP_Fx29() {
	// Set I = location of sprite for digit Vx.
	sf::Uint8 vx = (opcode & 0x0F00) >> 8;
	index = FONTSET_START_ADDRESS + registers[vx] * 5;
}

void Chip8::OP_Fx33() {
	// Store BCD representation of Vx in memory locations I, I+1, and I+2.
	sf::Uint8 vx = (opcode & 0x0F00) >> 8;
	sf::Uint8 value = registers[vx];
	RAM[index] = value / 100;
	RAM[index + 1] = (value % 100) / 10;
	RAM[index + 2] = value % 10;
}

void Chip8::OP_Fx55() {
	// Store registers V0 through Vx in memory starting at location I.
	sf::Uint8 vx = (opcode & 0x0F00) >> 8;
	for (int i = 0; i <= vx; i++) {
		RAM[index + i] = registers[i];
	}
}

void Chip8::OP_Fx65() {
	// Read registers V0 through Vx from memory starting at location I.
	sf::Uint8 vx = (opcode & 0x0F00) >> 8;
	for (int i = 0; i <= vx; i++) {
		registers[i] = RAM[index + i];
	}
}

void Chip8::OP_NULL() {}