CXX = g++
CXXFLAGS = -Wall 

main: main.o chip.o
	$(CXX) $(CXXFLAGS) -o chip main.o chip.o `sdl2-config --cflags --libs`

chip.o: chip.h


