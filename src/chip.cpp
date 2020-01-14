#include "chip.hpp"

uint8_t fontset[FONTSET_SIZE] =
{
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

Chip8::Chip8()
    : randGen(std::chrono::system_clock::now().time_since_epoch().count())
{
    // Initialize PC
    pc = START_ADDRESS;

    // Load fonts into memory
    for (unsigned int i = 0; i < FONTSET_SIZE; ++i)
    {
        memory[FONTSET_START_ADDRESS + i] = fontset[i];
    }

    // Initialize RNG
    randByte = std::uniform_int_distribution<unsigned short>(0, 255U);

}
void Chip8::LoadROM(char const* filename){
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if(file.is_open()){
        std::cout << "File opened\n" << std::endl;
        std::streampos size = file.tellg();
        char* buffer = new char[size];

        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();

        for(long i = 0; i < size; ++i){
            memory[START_ADDRESS + i] = buffer[i];
        }

        delete[] buffer;
    }
    else {
        std::cout << "Error opening file.\n" << std::endl;
		std::cout << filename << std::endl;
        std::exit(-1);
    }
}


void Chip8::Cycle(){

    // Splitting up opcode into high and low significant bytes
    opcode = (memory[pc] << 8u) | memory[pc + 1];

    std::uint8_t high = memory[pc];
    std::uint8_t low = memory[pc + 1];

    std::uint8_t x = high & 0x0F;
    std::uint8_t y = low >> 4;
    std::uint8_t sum; std::uint8_t key; std::uint8_t i;
    pc += 2;

    switch (high & 0xF0) {
    case 0x00:
        switch (low) {
        /* 00E0 - CLS */
        case 0xE0:
            memset(video, 0, VIDEO_HEIGHT * VIDEO_WIDTH);
            break;
        /* 00EE - RET */
        case 0xEE:
            --sp;
            pc = stack[sp];
            break;
        //default:
            //report code functionality
        }
        break;

    /* 1nnn - JP addr */
    case 0x10:
        pc = getNNN(high, low);
        break;

    /* 2nnn - CALL addr */
    case 0x20:
        stack[sp] = pc;
        ++sp;
        pc = getNNN(high, low);
        break;

    /* 3xkk - SE Vx, byte */
    case 0x30:
        if (registers[x] == low)
            pc += 2;
        break;

    /* 4xkk - SNE Vx, byte */
    case 0x40:
        if (registers[x] != low)
            pc += 2;
        break;

    /* 5xy0 - SE Vx, Vy */
    case 0x50:
        if (registers[x] == registers[y])
            pc += 2;
        break;

    /* 6xkk - LD Vx, byte */
    case 0x60:
        registers[x] = low;
        break;

    /* 7xkk - ADD Vx, byte */
    case 0x70:
        registers[x] += low;
        break;

    /* 8xy0 - LD Vx, Vy */
    case 0x80:
        switch (low & 0x0F) {
        case 0x00:
            registers[x] = registers[y];
            break;
        case 0x01:
            registers[x] |= registers[y];
            break;
        case 0x02:
            registers[x] &= registers[y];
            break;
        case 0x03:
            registers[x] ^= registers[y];
            break;
        case 0x04:
            sum = registers[x] + registers[y];
            registers[0xF] = (sum > 255) ? 1 : 0;
            registers[x] = sum & 0x00FF;
            break;
        case 0x05:
            registers[0xF] = (registers[x] > registers[y]) ? 1 : 0;
            registers[x] -= registers[y];
            break;
        case 0x06:
            registers[0xF] = registers[y] & 1;
            registers[x] >>= 1;
            break;
        case 0x07:
            registers[0xF] = (registers[y] > registers[x]) ? 1 : 0;
            registers[x] = registers[y] - registers[x];
            break;
        case 0x0E:
            registers[0xF] = (registers[y] & 128) >> 7;
            registers[x] <<= 1;
            break;
        }
        break;
    case 0x90:
        if (registers[x] != registers[y])
            pc += 2;
        break;
    case 0xA0:
        index = getNNN(high, low);
        break;
    case 0xB0:
        pc = getNNN(high, low) + registers[0];
        break;
    case 0xC0:
        registers[x] = randByte(randGen) & low;
        break;
    case 0xD0:
        OP_DXYN();
        break;
    case 0xE0:
        switch (low) {
        case 0x9E:
            key = registers[x];
            if (keypad[key]) {
                pc += 2;
            }
            break;
        case 0xA1:
            key = registers[x];
            if (!keypad[key]) {
                pc += 2;
            }
            break;
        }
        break;
    case 0xF0:
        switch (low) {
        case 0x07:
            registers[x] = delayTimer;
            break;
        case 0x0A:
            if (keypad[0])
                registers[x] = 0;
            else if (keypad[1])
                registers[x] = 1;
            else if (keypad[2])
                registers[x] = 2;
            else if (keypad[3])
                registers[x] = 3;
            else if (keypad[4])
                registers[x] = 4;
            else if (keypad[5])
                registers[x] = 5;
            else if (keypad[6])
                registers[x] = 6;
            else if (keypad[7])
                registers[x] = 7;
            else if (keypad[8])
                registers[x] = 8;
            else if (keypad[9])
                registers[x] = 9;
            else if (keypad[10])
                registers[x] = 10;
            else if (keypad[11])
                registers[x] = 11;
            else if (keypad[12])
                registers[x] = 12;
            else if (keypad[13])
                registers[x] = 13;
            else if (keypad[14])
                registers[x] = 14;
            else if (keypad[15])
                registers[x] = 15;
            else
                pc -= 2;
            break;
        case 0x15:
            delayTimer = registers[x];
            break;
        case 0x18:
            soundTimer = registers[x];
            break;
        case 0x1E:
            index += registers[x];
            break;
        case 0x29:
            index = FONTSET_START_ADDRESS + (5 * registers[x]);
            break;
        case 0x33:
            //Ones-place
            memory[index + 2] = registers[x] % 10;
            registers[x] /= 10;
            //Tens-place
            memory[index + 1] = registers[x] % 10;
            registers[x] /= 10;
            //Hundreds-place
            memory[index] = registers[x] % 10;
            break;
        case 0x55:
            for (i = 0; i <= x; ++i) {
                memory[index + i] = registers[i];
            }
            break;
        case 0x65:
            for (i = 0; i <= x; ++i) {
                registers[i] = memory[index + i];
            }
            break;
        }
        break;
    }
    if (delayTimer > 0)
    {
        --delayTimer;
    }

    // Decrement the sound timer if it's been set
    if (soundTimer > 0)
    {
        --soundTimer;
    }

}

// Helper methods
std::uint16_t Chip8::getNNN(std::uint8_t high, std::uint8_t low) {
    std::uint16_t nnn = high & 0x0F;
    nnn <<= 8;
    nnn += low;
    return nnn;
}

void Chip8::OP_DXYN() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    uint8_t height = opcode & 0x000Fu;

    // Wrap if going beyond screen boundaries
    uint8_t xPos = registers[Vx] % VIDEO_WIDTH;
    uint8_t yPos = registers[Vy] % VIDEO_HEIGHT;

    registers[0xF] = 0;
    for (unsigned int row = 0; row < height; ++row) {
        uint8_t spriteByte = memory[index + row];
        for (unsigned int col = 0; col < 8; ++col) {
            uint8_t spritePixel = spriteByte & (0x80u >> col);
            uint32_t* screenPixel = &video[(yPos + row) * VIDEO_WIDTH + (xPos + col)];

            // Sprite pixel is on
            if (spritePixel) {
                // Screen pixel also - collision
                if (*screenPixel == 0xFFFFFFFF) {
                    registers[0xF] = 1;
                }
                // Effectively XOR with the sprite pixel
                *screenPixel ^= 0xFFFFFFFF;
            }
        }
    }
}
