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
