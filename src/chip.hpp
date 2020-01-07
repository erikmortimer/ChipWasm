#ifndef chip_h
#define chip_h

#include <fstream>
#include <iostream>
#include <cstring>
#include <cstdint>
#include <algorithm>
#include <chrono>
#include <random>
#include <ctime>
#include <SDL.h>

const unsigned int START_ADDRESS = 0x200;
const unsigned int FONTSET_START_ADDRESS = 0x50;
const unsigned int FONTSET_SIZE = 80;
const unsigned int VIDEO_WIDTH = 64;
const unsigned int VIDEO_HEIGHT = 32;

class Chip8{
	public:
		Chip8();
		std::uint8_t keypad[16];
        std::uint32_t video[64 * 32];

		void LoadROM(char const* filename);
		void Cycle();

        // Helper method for XNNN instructions
        std::uint16_t getNNN(uint8_t high, uint8_t low);
        void OP_DXYN();
	private:
        std::uint8_t registers[16]{};
        std::uint8_t memory[4096]{};
        
        std::uint16_t index{};
        std::uint16_t pc{};
        std::uint16_t stack[16]{};
        std::uint8_t sp{};
        std::uint8_t delayTimer{};
        std::uint8_t soundTimer{};
        std::uint16_t opcode{};

        std::default_random_engine randGen;
        std::uniform_int_distribution<unsigned short> randByte{ 0, 255 };
    };

#endif
