CXX = g++
CXXFLAGS = -Wall 

chip: chip.o
	$(CXX) $(CXXFLAGS) -o chip chip.o

chip.o: chip.cpp chip.h
	$(CXX) $(CXXFLAGS) -c chip.cpp
