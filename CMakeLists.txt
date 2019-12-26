cmake_minimum_required(VERSION 3.1)
project(ChipWasm)

find_package(SDL2 REQUIRED)
include_directories(include ${SDL2_INCLUDE_DIRS})

file(GLOB SOURCES "src/*.cpp")
add_executable(ChipWasm ${SOURCES})

target_link_libraries(ChipWasm ${SDL2_LIBRARIES})
