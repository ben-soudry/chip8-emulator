#include <string>
#include <stdio.h>
#include "chip8.hpp"
#include <assert.h>

void testJump(){
    Chip8 chip8("roms/games/Airplane.ch8");
    chip8.runInstruction(0x1123);
    assert(chip8.PC == 0x123);
}

void testJumpOffset(){
    Chip8 chip8("roms/games/Airplane.ch8");
    chip8.V[0] = 0x10;
    chip8.runInstruction(0xB123);
    assert(chip8.PC == 0x133);
}

void testCallAndReturn(){
    Chip8 chip8("roms/games/Airplane.ch8");
    //Function call at address 0x300
    uint16_t return_address = chip8.PC;
    
    chip8.runInstruction(0x2300);
    assert(chip8.PC == 0x300);
    assert(chip8.stack_ptr == 0);
    assert(chip8.stack[0] == return_address);
    
    //Return
    chip8.runInstruction(0x00EE);
    assert(chip8.PC == return_address);
    assert(chip8.stack_ptr == -1);
}
void testSkip(){
    Chip8 chip8("roms/games/Airplane.ch8");
    uint16_t curr_address = chip8.PC;
    chip8.V[7] = 0x42;
    chip8.runInstruction(0x3742);
    assert(chip8.PC == curr_address + 4);
}

void testNoSkip(){
    Chip8 chip8("roms/games/Airplane.ch8");
    uint16_t curr_address = chip8.PC;
    chip8.V[7] = 0x81;
    chip8.runInstruction(0x3742);
    assert(chip8.PC == curr_address + 2);
}

void testLoadByte(){
    Chip8 chip8("roms/games/Airplane.ch8");
    chip8.runInstruction(0x6342);
    assert(chip8.V[3] == 0x42);
}

void testAddByte(){
    Chip8 chip8("roms/games/Airplane.ch8");
    chip8.V[3] = 2;
    //2 + 8 = 10
    chip8.runInstruction(0x7308);
    assert(chip8.V[3] == 10);
}

void testAddxy(){
    Chip8 chip8("roms/games/Airplane.ch8");
    chip8.V[3] = 3;
    chip8.V[5] = 4;
    //3+ 4 = 7
    chip8.runInstruction(0x8354);
    assert(chip8.V[3] == 7);
    //Check no overflow flag set
    assert(chip8.V[0xF] == 0); 
}

void testAddxyOverflow(){
    Chip8 chip8("roms/games/Airplane.ch8");
    chip8.V[3] = 0xFF;
    chip8.V[5] = 0x3;
    //255 + 3 = 2, with overflow flag set
    chip8.runInstruction(0x8354);
    assert(chip8.V[3] == 2);
    //Check overflow flag set
    assert(chip8.V[0xF] == 1); 
}

void testSubxy(){
    Chip8 chip8("roms/games/Airplane.ch8");
    chip8.V[3] = 11;
    chip8.V[5] = 3;
    //11-3 = 8
    chip8.runInstruction(0x8355);
    assert(chip8.V[3] == 8);
    //Check no borrow flag set
    assert(chip8.V[0xF] == 1); 
}

void testSubxyBorrow(){
    Chip8 chip8("roms/games/Airplane.ch8");
    chip8.V[3] = 2;
    chip8.V[5] = 3;
    //2-3 = 255 (-1)
    chip8.runInstruction(0x8355);
    assert(chip8.V[3] == 0xFF);
    //Check no borrow flag set
    assert(chip8.V[0xF] == 0); 
}

void testLoadedDigits(){
    Chip8 chip8("roms/games/Airplane.ch8");
    assert(chip8.memory[0] == 0xF0);
    assert(chip8.memory[1] == 0x90);
    assert(chip8.memory[2] == 0x90);
    assert(chip8.memory[3] == 0x90);
    assert(chip8.memory[4] == 0xF0);
}


int main()
{
    printf("Chip8 Unit Tests... \n");

    testJump();
    testJumpOffset();
    
    
    testCallAndReturn();
    testSkip();
    testNoSkip();
    testLoadByte();
    testAddByte();
    testAddxy();
    testAddxyOverflow();
    testSubxy();
    testSubxyBorrow();
    testLoadedDigits();

    printf("All Tests Pass! \n");
}


