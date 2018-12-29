LDLIBS=-lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -pthread

all: chip8-app chip8-test chip8-wasm

chip8-wasm: chip8.cpp chip8_binding.cpp 
	emcc --bind -o chip8.js chip8_binding.cpp chip8.cpp --embed-file roms/

chip8-app: main.o chip8.o
	g++ -std=c++11 main.o chip8.o -o chip8-app $(LDLIBS)

chip8-test: test.o chip8.o
	g++ -std=c++11 test.o chip8.o -o chip8-test $(LDLIBS)

main.o: main.cpp
	g++ -std=c++11 -c main.cpp

chip8.o: chip8.cpp
	g++ -std=c++11 -c chip8.cpp

test.o: test.cpp
	g++ -std=c++11 -c test.cpp

