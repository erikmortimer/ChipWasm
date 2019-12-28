#ifndef chip_h
#define chip_h

#include <fstream>
#include <cstring>
#include <cstdint>
#include <chrono>
#include <random>
#include <ctime>
#include <SDL2/SDL.h>

const unsigned int START_ADDRESS = 0x200;
const unsigned int FONTSET_START_ADDRESS = 0x50;
const unsigned int FONTSET_SIZE = 80;
const unsigned int VIDEO_WIDTH = 64;
const unsigned int VIDEO_HEIGHT = 32;

class Chip8{
	public:
		Chip8();
		uint8_t keypad[16]{};
        uint32_t video[64 * 32]{};

		void LoadROM(char const* filename);
		void Cycle();
	private:
		void Table0();
		void Table8();
		void TableE();
		void TableF();
		
		// Instructions
		void OP_NULL();
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

		uint8_t registers[16]{};
        uint8_t memory[4096]{};
        
        uint16_t index{};
        uint16_t pc{};
        uint16_t stack[16]{};
        uint8_t sp{};
        uint8_t delayTimer{};
        uint8_t soundTimer{};
        uint16_t opcode{};
		
		// Tables
		typedef void (Chip8::*Chip8Func)();
		Chip8Func table[0xF + 1]{&Chip8::OP_NULL};
		Chip8Func table0[0xE + 1]{&Chip8::OP_NULL};
		Chip8Func table8[0xE + 1]{&Chip8::OP_NULL};
		Chip8Func tableE[0xE + 1]{&Chip8::OP_NULL};
		Chip8Func tableF[0x65 + 1]{&Chip8::OP_NULL};

    };

class Platform{
	private:
		SDL_Window* window;
		SDL_Renderer* renderer;
		SDL_Texture* texture;
    public:
        Platform(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight){
            SDL_Init(SDL_INIT_VIDEO);
			window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
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

#endif
