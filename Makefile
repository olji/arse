SOURCES=$(wildcard src/*.cpp)

all: 
	g++ $(SOURCES) -g -o arse
