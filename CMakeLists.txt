cmake_minimum_required(VERSION 3.1)
project(ChipWasm)

include_directories(include)

file(GLOB SOURCES "src/*.cpp")
add_executable(chip ${SOURCES})
