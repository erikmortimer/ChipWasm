#include "chip.h"

Chip8::Chip8(){
    // Init PC
    pc = START_ADDRESS;

    // Load the Font into memory
    for(unsigned int i = 0; i < FONTSET_SIZE; ++i){
        memory[FONTSET_START_ADDRESS + i] = fontset[i];
    }
}

void Chip8::LoadROM(char const* filename){
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if(file.is_open()){
        // Get size of file and allocate buffer to that size
        std::streampos size = file.tellg();
        char* buffer = new char[size];

        // Start at beginning of the file, and fill the buffer
        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();

        // Load ROM into Chip8 memory
        for(long i = 0; i < size; i++){
            memory[START_ADDRESS + i] = buffer[i];
        }

        // Free the buffer
        delete[] buffer;
    }
}

// 00E0: CLS
// Clear the display
void Chip8::OP_00E0(){
	memset(video, 0, VIDEO_HEIGHT * VIDEO_WIDTH);
}

// 00EE: RET
// Return from a subroutine
void Chip8::OP_00EE(){
	--sp;
	pc = stack[sp];
}

// 1nnn: JP addr
// Jump to location nnn
void Chip8::OP_1nnn(){
	uint16_t address = opcode & 0x0FFFu;
	pc = address;
}

// 2nnn: CALL addr
// Call subroutine at nnn
void Chip8::OP_2nnn(){
	uint16_t address = opcode & 0x0FFFu;
	stack[sp] = pc;
	++sp;
	pc = address;
}

// 3xkk: SE Vx, byte
// Skip next instruction if Vx = kk
void Chip8::OP_3xkk(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;
	if (registers[Vx] == byte){
		pc += 2;
	}
}

// 4xkk: SNE Vx, byte
// Skip next instruction if Vx != kk
void Chip8::OP_4xkk(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;
	if(registers[Vx] != byte){
		pc += 2;
	}
}

// 5xy0: SE Vx, Vy
// Skip next instruction if Vx = Vy
void Chip8::OP_5xy0(){
	uint8_t Vx = (opcode * 0x0F00u) >> 8u;
	uint8_t Vy = (opcode * 0x00F0u) >> 4u;
	if (registers[Vx] == registers[Vy]){
		pc += 2;
	}
}

// 6xkk: LD Vx, byte
// Set Vx = kk
void Chip8::OP_6xkk(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = (opcode & 0x00FFu);
	registers[Vx] = byte;
}

// 7xkk: ADD Vx, byte
// Set Vx = Vx + kk
void Chip8::OP_7xkk(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t	byte = opcode & 0x00FFu;
	registers[Vx] += byte;
}

// 8xy0: LD Vx, Vy
// Set Vx = Vy
void Chip8::OP_8xy0(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	registers[Vx] = registers[Vy];
}

// 8xy1: OR Vx, Vy
// Set Vx = Vx OR Vy
void Chip8::OP_8xy1(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	registers[Vx] != registers[Vy];
}

// 8xy2: AND Vx, Vy
// Set Vx = Vx AND Vy
void Chip8::OP_8xy2(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	registers[Vx] &= registers[Vy];	
}

// 8xy3: XOR Vx, Vy
// Set Vx = Vx XOR Vy
void Chip8::OP_8xy3(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	registers[Vx] ^= registers[Vy];
}
