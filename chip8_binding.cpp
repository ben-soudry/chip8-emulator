#include "chip8.hpp"
#include <emscripten/bind.h>
using namespace emscripten;

EMSCRIPTEN_BINDINGS(my_class_example) {
    class_<Chip8>("Chip8")
        .constructor<std::string>()
        .function("setKey", &Chip8::setKey)
        .function("tickClock", &Chip8::tickClock)
        .function("isSoundOn", &Chip8::isSoundOn)
        .function("emulateCycle", &Chip8::emulateCycle)
        .function("runInstruction", &Chip8::runInstruction)
        .function("printRom", &Chip8::printRom)
        ;    
}

