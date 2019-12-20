#pragma once

#include <fstream>
#include <chrono>
#include <random>

const unsigned int START_ADDRESS = 0x200;
const unsigned int FONTSET_START_ADDRESS = 0x50;
const unsigned int FONTSET_SIZE = 80;

class Chip8{
    private:
        uint8_t registers[16]{};
        uint8_t memory[4096]{};
        uint8_t fontset[FONTSET_SIZE] = {
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
        uint16_t index{};
        uint16_t pc{};
        uint16_t stack[16]{};
        uint8_t sp{};
        uint8_t delayTimer{};
        uint8_t soundTimer{};
        uint8_t keypad[16]{};
        uint32_t video[64 * 32]{};
        uint16_t opcode;
        std::default_random_engine randGen;
        std::uniform_int_distribution<uint8_t> randByte;
    public:
        Chip8() : randGen(std::chrono::system_clock::now().time_since_epoch().count()){
            // Init RNG
            randByte = std::uniform_int_distribution<uint8_t>(0, 255U);
        }
        void LoadROM(char const* filename);

		// Instructions
		void OP_00E0(); // CLS: Clear the display
		void OP_00EE(); // RET: Return from a subroutine
		void OP_1nnn(); // 1nnn: JP addr
		void OP_2nnn(); // 2nnn: CALL addr
		void OP_3xkk(); // 3xkk: SE Vx, byte
		void OP_4xkk(); // 4xkk: SNE Vx, byte
		void OP_5xy0(); // 5xy0: SE Vx, Vy
		void OP_6xkk(); // 6xkk: LD Vx, byte
		void OP_7xkk(); // 7xkk: ADD Vx, byte
		void OP_8xy0(); // 8xy0: LD Vx, Vy
		void OP_8xy1(); // 8xy1: OR Vx, Vy
		void OP_8xy2(); // 8xy2: AND Vx, Vy
		void OP_8xy3(); // 8xy3: XOR Vx, Vy
		void OP_8xy4(); // 8xy4: ADD Vx, Vy
};
