// Chip8 Emulator
// @author Erik Mortimer
// @cited

#include <chrono>
#include <iostream>
#include "chip.hpp"
#include "platform.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

void main_loop() {
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(emscripten_loop, 60, 1);
#endif
    Platform platform("Chip-8 Emulator", VIDEO_WIDTH * 10, VIDEO_HEIGHT * 10, VIDEO_WIDTH, VIDEO_HEIGHT);
    Chip8 chip8;
    chip8.LoadROM("../../test_opcode.ch8");
    int cycleDelay = 1;
    int videoPitch = sizeof(chip8.video[0]) * VIDEO_WIDTH;
    auto lastCycleTime = std::chrono::high_resolution_clock::now();
    bool quit = false;

    while (!quit) {
        quit = platform.ProcessInput(chip8.keypad);

        auto currentTime = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();
        if (dt > cycleDelay) {
            lastCycleTime = currentTime;
            chip8.Cycle();
            platform.Update(chip8.video, videoPitch);
        }
    }
}

#ifdef __EMSCRIPTEN__
void emscripten_loop() {
    Platform platform("Chip-8 Emulator", VIDEO_WIDTH * 10, VIDEO_HEIGHT * 10, VIDEO_WIDTH, VIDEO_HEIGHT);
    Chip8 chip8;
    chip8.LoadROM("test_opcode.ch8");
    int cycleDelay = 3;
    int videoPitch = sizeof(chip8.video[0]) * VIDEO_WIDTH;
    auto lastCycleTime = std::chrono::high_resolution_clock::now();
    bool quit = false;

    while (!quit) {
        quit = platform.ProcessInput(chip8.keypad);

        auto currentTime = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();
        if (dt > cycleDelay) {
            lastCycleTime = currentTime;
            chip8.Cycle();
            platform.Update(chip8.video, videoPitch);
        }
    }
}
#endif


int main(int argc, char* argv[]) {
    main_loop();
    return 0;
}
