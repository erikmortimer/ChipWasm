#pragma once

#include <fstream>
#include <cstring>
#include <chrono>
#include <random>

const unsigned int START_ADDRESS = 0x200;
const unsigned int FONTSET_START_ADDRESS = 0x50;
const unsigned int FONTSET_SIZE = 80;
const unsigned int VIDEO_WIDTH = 64;
const unsigned int VIDEO_HEIGHT = 32;

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

			// Init PC
    		pc = START_ADDRESS;

    		// Load the Font into memory
    		for(unsigned int i = 0; i < FONTSET_SIZE; ++i){
       			memory[FONTSET_START_ADDRESS + i] = fontset[i];
    		}
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
		void OP_8xy5(); // 8xy5: SUB Vx, Vy
		void OP_8xy6(); // 8xy6: SHR Vx
		void OP_8xy7(); // 8xy7: SUBN Vx, Vy
		void OP_8xyE(); // 8xyE: SHL Vx {, Vy}
		void OP_9xy0(); // 9xy0: SNE Vx, V	void OP_9xy0(); // 9xy0: SNE Vx, Vy
		void OP_Annn(); // Annn: LD I, addr
		void OP_Bnnn(); // Bnnn: JP V0, addr
		void OP_Cxkk(); // Cxkk: RND Vx, byte
		void OP_Dxyn(); // Dxyn: DRW Vx, Vy, nibble
		void OP_Ex9E(); // Ex9E: SKP Vx
		void OP_ExA1(); // ExA1: SKNP Vx
		void OP_Fx07(); // Fx07: LD Vx, DT
		void OP_Fx0A(); // Fx0A: LD Vx, K
		void OP_Fx15(); // Fx15: LD DT, Vx
		void OP_Fx18(); // Fx18: LD ST, Vx
		void OP_Fx1E(); // Fx1E: ADD I, Vx
		void OP_Fx29(); // Fx29: LD F, Vx
		void OP_Fx33(); // Fx33: LD B, Vx
		void OP_Fx55(); // Fx55: LD [I], Vx
		void OP_Fx65(); // Fx65: LD Vx, [I]
};
