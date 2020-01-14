# ChipWasm
A Chip-8 Emulator that runs in the browser using WebAssembly

# Compiling with Emscripten
```
emcc ../src/chip.cpp ../src/main.cpp -std=c++11 -O3 --preload-file games/ -s USE_SDL=2 -s EXPORTED_FUNCTIONS='["_main"]' -s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]'
```

# Current state
STILL UNDER DEVELOPMENT
Had to change from using function pointers to switch statements due to webassembly issues.
