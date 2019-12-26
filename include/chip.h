#pragma once

#include <fstream>
#include <cstring>
#include <cstdint>
#include <chrono>
#include <random>
#include <SDL2/SDL.h>

const unsigned int START_ADDRESS = 0x200;
const unsigned int FONTSET_START_ADDRESS = 0x50;
const unsigned int FONTSET_SIZE = 80;
const unsigned int VIDEO_WIDTH = 64;
const unsigned int VIDEO_HEIGHT = 32;

class Chip8{
    public:
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

		void Table0(){
			((*this).*(table0[opcode & 0x000Fu]))();
		}
		void Table8(){
			((*this).*(table8[opcode & 0x000Fu]))();
		}
		void TableE(){
			((*this).*(tableE[opcode & 0x000Fu]))();
		}
		void TableF(){
			((*this).*(tableF[opcode & 0x00FFu]))();
		}

		void OP_NULL(){}

		// Tables
		typedef void (Chip8::*Chip8Func)();
		Chip8Func table[0xF + 1]{&Chip8::OP_NULL};
		Chip8Func table0[0xE + 1]{&Chip8::OP_NULL};
		Chip8Func table8[0xE + 1]{&Chip8::OP_NULL};
		Chip8Func tableE[0xE + 1]{&Chip8::OP_NULL};
		Chip8Func tableF[0x65 + 1]{&Chip8::OP_NULL};

        Chip8() : randGen(std::chrono::system_clock::now().time_since_epoch().count()){
            // Init RNG
            randByte = std::uniform_int_distribution<uint8_t>(0, 255U);

			// Init PC
    		pc = START_ADDRESS;

    		// Load the Font into memory
    		for(unsigned int i = 0; i < FONTSET_SIZE; ++i){
       			memory[FONTSET_START_ADDRESS + i] = fontset[i];
    		}

			// Set up function pointer table
			table[0x0] = &Chip8::Table0;
			table[0x1] = &Chip8::OP_1nnn;
			table[0x2] = &Chip8::OP_2nnn;
			table[0x3] = &Chip8::OP_3xkk;
			table[0x4] = &Chip8::OP_4xkk;
			table[0x5] = &Chip8::OP_5xy0;
			table[0x6] = &Chip8::OP_6xkk;
			table[0x7] = &Chip8::OP_7xkk;
			table[0x8] = &Chip8::Table8;
			table[0x9] = &Chip8::OP_9xy0;
			table[0xA] = &Chip8::OP_Annn;
			table[0xB] = &Chip8::OP_Bnnn;
			table[0xC] = &Chip8::OP_Cxkk;
			table[0xD] = &Chip8::OP_Dxyn;
			table[0xE] = &Chip8::TableE;
			table[0xF] = &Chip8::TableF;

			table0[0x0] = &Chip8::OP_00E0;
			table0[0xE] = &Chip8::OP_00EE;

			table8[0x0] = &Chip8::OP_8xy0;
			table8[0x1] = &Chip8::OP_8xy1;
			table8[0x2] = &Chip8::OP_8xy2;
			table8[0x3] = &Chip8::OP_8xy3;
			table8[0x4] = &Chip8::OP_8xy4;
			table8[0x5] = &Chip8::OP_8xy5;
			table8[0x6] = &Chip8::OP_8xy6;
			table8[0x7] = &Chip8::OP_8xy7;
			table8[0xE] = &Chip8::OP_8xyE;

			tableE[0x1] = &Chip8::OP_ExA1;
			tableE[0xE] = &Chip8::OP_Ex9E;

			tableF[0x07] = &Chip8::OP_Fx07;
			tableF[0x0A] = &Chip8::OP_Fx0A;
			tableF[0x15] = &Chip8::OP_Fx15;
			tableF[0x18] = &Chip8::OP_Fx18;
			tableF[0x1E] = &Chip8::OP_Fx1E;
			tableF[0x29] = &Chip8::OP_Fx29;
			tableF[0x33] = &Chip8::OP_Fx33;
			tableF[0x55] = &Chip8::OP_Fx55;
			tableF[0x65] = &Chip8::OP_Fx65;

        }
        void LoadROM(char const* filename);
		void Cycle();

		// Instructions
		void OP_00E0(); // CLS: Clear the display
		void OP_00EE(); // RET: Return from a subroutine
		void OP_1nnn(); // 1nnn: JP addr
		void OP_2nnn(); // 2nnn: CALL addr
		void OP_3xkk(); // 3xkk: SE Vx, byte
		void OP_4xkk(); // 4xkk: SNE Vx, byte
		void OP_5xy0(); // 5xy0: SE Vx, Vy -----
		void OP_6xkk(); // 6xkk: LD Vx, byte
		void OP_7xkk(); // 7xkk: ADD Vx, byte
		void OP_8xy0(); // 8xy0: LD Vx, Vy
		void OP_8xy1(); // 8xy1: OR Vx, Vy -------
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

class Platform{
	private:
		SDL_Window* window;
		SDL_Renderer* renderer;
		SDL_Texture* texture;
    public:
        Platform(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight){
            SDL_Init(SDL_INIT_VIDEO);
			window = SDL_CreateWindow(title, 0, 0, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
			texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, textureWidth, textureHeight);
        }
		~Platform(){
			SDL_DestroyTexture(texture);
			SDL_DestroyRenderer(renderer);
			SDL_DestroyWindow(window);
			SDL_Quit();
		}
		void Update(void const* buffer, int pitch){
			SDL_UpdateTexture(texture, nullptr, buffer, pitch);
			SDL_RenderClear(renderer);
			SDL_RenderCopy(renderer, texture, nullptr, nullptr);
			SDL_RenderPresent(renderer);
		}
		bool ProcessInput(uint8_t* keys){
			bool quit = false;
			SDL_Event event;
			while(SDL_PollEvent(&event)){
				switch(event.type){
					case SDL_QUIT:
						quit = true;
						break;
					case SDL_KEYDOWN:
						switch(event.key.keysym.sym){
							case SDLK_ESCAPE:
								quit = true;
								break;
							case SDLK_x:
								keys[0] = 1;
								break;
							case SDLK_1:
								keys[1] = 1;
								break;

							case SDLK_2:
								keys[2] = 1;
								break;

							case SDLK_3:
								keys[3] = 1;
								break;

							case SDLK_q:
								keys[4] = 1;
								break;

							case SDLK_w:
								keys[5] = 1;
								break;

							case SDLK_e:
								keys[6] = 1;
								break;

							case SDLK_a:
								keys[7] = 1;
								break;

							case SDLK_s:
								keys[8] = 1;
								break;

							case SDLK_d:
								keys[9] = 1;
								break;

							case SDLK_z:
								keys[0xA] = 1;
								break;

							case SDLK_c:
								keys[0xB] = 1;
								break;

							case SDLK_4:
								keys[0xC] = 1;
								break;

							case SDLK_r:
								keys[0xD] = 1;
								break;

							case SDLK_f:
								keys[0xE] = 1;
								break;

							case SDLK_v:
								keys[0xF] = 1;
								break;
						}
						break;
					case SDL_KEYUP:
						switch (event.key.keysym.sym){
							case SDLK_x:
								keys[0] = 0;
								break;

							case SDLK_1:
								keys[1] = 0;
								break;

							case SDLK_2:
								keys[2] = 0;
								break;

							case SDLK_3:
								keys[3] = 0;
								break;

							case SDLK_q:
								keys[4] = 0;
								break;

							case SDLK_w:
								keys[5] = 0;
								break;

							case SDLK_e:
								keys[6] = 0;
								break;

							case SDLK_a:
								keys[7] = 0;
								break;

							case SDLK_s:
								keys[8] = 0;
								break;

							case SDLK_d:
								keys[9] = 0;
								break;

							case SDLK_z:
								keys[0xA] = 0;
								break;

							case SDLK_c:
								keys[0xB] = 0;
								break;

							case SDLK_4:
								keys[0xC] = 0;
								break;

							case SDLK_r:
								keys[0xD] = 0;
								break;

							case SDLK_f:
								keys[0xE] = 0;
								break;

							case SDLK_v:
								keys[0xF] = 0;
								break;
						}
						break;
				}
			}
			return quit;
		}
};