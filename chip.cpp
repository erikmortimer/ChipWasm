#include "chip.h"

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

// 8xy4: ADD Vx, Vy
// Set Vx = Vx + Vy, set VF = carry
void Chip8::OP_8xy4(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	uint16_t sum = registers[Vx] + registers[Vy];

	if(sum > 255U)
		registers[0xF] = 1;
	else
		registers[0xF] = 0;

	registers[Vx] = sum & 0xFFu;
}

// 8xy5: SUB Vx, Vy
// Set Vx = Vx - Vy, set VF = NOT borrow
// If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx, and the results stored in Vx
void Chip8::OP_8xy5(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if(registers[Vx] > registers[Vy])
		registers[0xF] = 1;
	else
		registers[0xF] = 0;

	registers[Vx] -= registers[Vy];
}

// 8xy6: SHR Vx
// Set Vx= Vx SHR 1
// If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is divided by 2
// A right shift is performed (division by 2), and the least significant bit is saved in Register VF
void Chip8::OP_8xy6(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	//Save LSB in VF
	registers[0xF] = (registers[Vx] & 0x1u);
	registers[Vx] >>= 1;
}

// 8xy7: SUBN Vx, Vy
// Set Vx = Vy - Vx, set VF = NOT borrow
// If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from Vy, and the results stored in Vx.
void Chip8::OP_8xy7(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (registers[Vy] > registers[Vx])
		registers[0xF] = 1;
	else
		registers[0xF] = 0;

	registers[Vx] = registers[Vy] - registers[Vx];
}

// 8xyE: SHL Vx {, Vy}
// Set Vx = Vx SHL 1
// If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0. Then Vx is multiplied by 2.
// A left shift is performed (multiplication by 2), and the most significant bit is saved in Register VF.
void Chip8::OP_8xyE(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	// Save MSB in VF
	registers[0xF] = (registers[Vx] & 0x80u) >> 7u;

	registers[Vx] <<= 1;
}

// 9xy0: SNE Vx, Vy
// Skip next instruction if Vx != Vy
// Since our PC has already been incremented by 2 in Cycle(), we can just increment by 2 again to skip the next instruction.
void Chip8::OP_9xy0(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	if(registers[Vx] != registers[Vy])
		pc += 2;
}

// Annn: LD I, addr
// Set I = nnn
void Chip8::OP_Annn(){
	uint16_t address = opcode & 0x0FFFu;
	index = address;
}

// Bnnn: JP V0, addr
// Jump to location nnn + V0.
void Chip8::OP_Bnnn(){
	uint16_t address = opcode & 0x0FFFu;
	pc = registers[0] + address;
}

// Cxkk: RND Vx, byte
// Set Vx = random byte AND kk
void Chip8::OP_Cxkk(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;
	registers[Vx] = randByte(randGen) & byte;
}

// Dxyn: DRW Vx, Vy, nibble
// Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision
void Chip8::OP_Dxyn(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	uint8_t height = opcode & 0x000Fu;

	// Wrap if going beyond screen boundaries
	uint8_t xPos = registers[Vx] % VIDEO_WIDTH;
	uint8_t yPos = registers[Vy] % VIDEO_HEIGHT;

	registers[0xF] = 0;

	for(unsigned int row = 0; row < height; ++row){
		uint8_t spriteByte = memory[index + row];
		for(unsigned int col = 0; col < 8; ++col){
			uint8_t spritePixel = spriteByte & (0x80u >> col);
			uint32_t* screenPixel = &video[(yPos + row) * VIDEO_WIDTH + (xPos + col)];

			// Sprite pixel is on
			if(spritePixel){
				// Screen pixel also on - collision
				if(*screenPixel == 0xFFFFFFFF){
					registers[0xF] = 1;
				}
				// Effectively XOR with the sprite pixel
				*screenPixel ^= 0xFFFFFFFF;
			}
		}
	}
}

// Ex9E: SKP Vx
// Skip next instruction if key with the value of Vx is pressed
void Chip8::OP_Ex9E(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t key = registers[Vx];
	if(keypad[key])
		pc += 2;
}

// ExA1: SKNP Vx
// Skip next instruction if key with the value of Vx is not pressed
void Chip8::OP_ExA1(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t key = registers[Vx];
	if(!keypad[key])
		pc += 2;
}

// Fx07: LD Vx, DT
// Set Vx = delay timer value
void Chip8::OP_Fx07(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	registers[Vx] = delayTimer;
}

// Fx0A: LD Vx, K
// Wait for a key press, store the value of the key in Vx
// Easiest way to "wait" is to decrement the PC by 2 whenever a keypad value is not detected. This has the effect of running the same instruction repeatedly
void Chip8::OP_Fx0A(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	if(keypad[0])
		registers[Vx] = 0;
	else if(keypad[1])
		registers[Vx] = 1;
	else if(keypad[2])
		registers[Vx] = 2;
	else if(keypad[3])
		registers[Vx] = 3;
	else if(keypad[4])
		registers[Vx] = 4;
	else if(keypad[5])
		registers[Vx] = 5;
	else if(keypad[6])
		registers[Vx] = 6;
	else if(keypad[7])
		registers[Vx] = 7;
	else if(keypad[8])
		registers[Vx] = 8;
	else if(keypad[9])
		registers[Vx] = 9;
	else if(keypad[10])
		registers[Vx] = 10;
	else if(keypad[11])
		registers[Vx] = 11;
	else if(keypad[12])
		registers[Vx] = 12;
	else if(keypad[13])
		registers[Vx] = 13;
	else if(keypad[14])
		registers[Vx] = 14;
	else if(keypad[15])
		registers[Vx] = 15;
	else
		pc -= 2;
}

// Fx15: LD DT, Vx
// Set delay timer = Vx
void Chip8::OP_Fx15(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	delayTimer = registers[Vx];
}

// Fx18: LD ST, Vx
// Set sound timer = Vx
void Chip8::OP_Fx18(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	soundTimer = registers[Vx];
}

// Fx1E: ADD I, Vx
// Set I = I + Vx
void Chip8::OP_Fx1E(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	index += registers[Vx];
}

// Fx29: LD F, Vx
// Set I = location of sprite for digit Vx
// We know the font characters are located at 0x50, and we know they're five bytes each, so we can get the address of the first byte of any character by taking an offset from the start address
void Chip8::OP_Fx29(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t digit = registers[Vx];
	index = FONTSET_START_ADDRESS + (5 * digit);
}

// Fx33: LD B, Vx
// Store BCD representation of Vx in memory locations I, I+1, and I+2
void Chip8::OP_Fx33(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t value = registers[Vx];

	// Ones-place
	memory[index + 2] = value % 10;
	value /= 10;

	// Tens-place
	memory[index + 1] = value % 10;
	value /= 10;

	// Hundreds-place
	memory[index] = value % 10;
}

// Fx55: LD [I], Vx
// Store registers V0 through Vx in memory starting at location I
void Chip8::OP_Fx55(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	for(uint8_t i = 0; i <= Vx; ++i)
		memory[index + i] = registers[i];
}

// Fx65: LD Vx, [I]
// Read registers V0 through Vx from memory starting at location I
void Chip8::OP_Fx65(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	for(uint8_t i = 0; i <= Vx; ++i)
		registers[i] = memory[index + i];
}
