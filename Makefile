LDLIBS=-lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -pthread

all: chip8-app chip8-test chip8-rl chip8-wasm

chip8-rl: main_rl.o chip8.o rl_pong.o
	g++ -std=c++11 main_rl.o rl_pong.o chip8.o -o chip8-rl $(LDLIBS)

chip8-wasm: chip8.cpp chip8_binding.cpp 
	emcc --bind -o chip8.js chip8_binding.cpp chip8.cpp --embed-file roms/

chip8-app: main.o chip8.o
	g++ -std=c++11 main.o chip8.o -o chip8-app $(LDLIBS)

chip8-test: test.o chip8.o
	g++ -std=c++11 test.o chip8.o -o chip8-test $(LDLIBS)

main.o: main.cpp
	g++ -std=c++11 -c main.cpp

chip8.o: chip8.cpp chip8.hpp
	g++ -std=c++11 -c chip8.cpp

test.o: test.cpp
	g++ -std=c++11 -c test.cpp

main_rl.o: main_rl.cpp 
	g++ -std=c++11 -c main_rl.cpp

rl_pong.o: rl_pong.cpp rl_pong.hpp
	g++ -std=c++11 -c rl_pong.cpp
