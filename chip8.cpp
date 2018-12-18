#include "chip8.hpp"

constexpr uint16_t Chip8::digitLocations[16]; 
constexpr uint8_t Chip8::digits[80]; 

Chip8::Chip8(std::string fileName)
        : m_fileName(fileName), PC(0x200)
{    
    //Load ROM into memory
    FILE* fp = fopen(m_fileName.c_str(), "r");
    if(fp == NULL){
        printf("Unable to read rom! \n");
        return;
    } else {
        unsigned short addr = 0x200; //ROM start location
        while(true) {
            int ch1 = fgetc(fp);
            int ch2 = fgetc(fp);
            if(ch1 == EOF || ch2 == EOF){
                break;
            }
            memory[addr] = (char) ch1;
            memory[addr+1] = (char) ch2;
            addr += 2;
        }
    }
    //Load Digits into memory
    memcpy(memory, digits, 80);
    
    //initialize registers
    this->initRegisters();

    //Clear display
    this->clearDisplay();
    //Set stack pointer to -1 (stack empty)
    stack_ptr = -1;
    //Set cycle count to 0;
    cycle_count = 0;
}

void Chip8::initRegisters(){
    for(int i=0; i < 16; i++){
        V[i] = 0;
    }
    ST = 0;
    DT = 0;
    I = 0;
}



void Chip8::emulateCycle(){
    uint8_t high_byte = memory[PC];
    uint8_t low_byte = memory[PC+1];
    uint16_t instr = ((uint16_t) high_byte << 8)  | (uint16_t) low_byte;

    this->runInstruction(instr);

    cycle_count++;
    if(cycle_count % tick_cycles == 0){
        if(DT > 0){
            DT--;
        }
        if(ST > 0) {
            ST--;
        }
    }
} 

void Chip8::clearDisplay(){
  for(int i=0; i< this->height; i++){
        for(int j=0; j< this->width; j++){
            display[i][j] = 0;
        }
    }
}
//Chip8 instruction implementations
//See Chip8 documentation for details:
//http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#2.4
void Chip8::CLS(){
    //Clear the display
    this->clearDisplay();
    PC += 2;
}

void Chip8::RET(){
    //Return from a subroutine
    if(stack_ptr == -1){
        printf("ERROR: RET called on empty stack \n");
        PC += 2;
        return;
    }        
    //Jump to return address on stack, decrement stack_top
    
    PC = stack[stack_ptr];
    stack_ptr--;
}

void Chip8::JP1(uint16_t addr){
    //Jump to an address
    PC = addr;
}

void Chip8::CALL(uint16_t addr){
    //Call a subroutine
    stack_ptr++;
    stack[stack_ptr] = PC;
    PC = addr;
}
void Chip8::SE1(uint8_t Vx, uint8_t byte){
    //Skip if equal
    if(V[Vx] == byte){
        //Increment the program counter twice to skip
        PC += 4;
    } else {
        PC += 2;
    }
}

void Chip8::SNE1(uint8_t Vx, uint8_t byte){
    //Skip if not equal
    if(V[Vx] != byte){
        //Increment the program counter twice to skip
        PC += 4;
    } else {
        PC += 2;
    }
}
void Chip8::SE2(uint8_t Vx, uint8_t Vy){
    //Skip if equal
    if(V[Vx] == V[Vy]){
        //Increment the program counter twice to skip
        PC += 4;
    } else {
        PC += 2;
    }
}
void Chip8::LD1(uint8_t Vx, uint8_t byte){
    V[Vx] = byte;
    PC += 2;
}
void Chip8::ADD1(uint8_t Vx, uint8_t byte){
    V[Vx] = V[Vx] + byte;
    PC += 2;
}
void Chip8::LD2(uint8_t Vx, uint8_t Vy){
    V[Vx] = V[Vy];
    PC += 2;
}

void Chip8::OR(uint8_t Vx, uint8_t Vy){
    //Bitwise OR
    V[Vx] = V[Vx] | V[Vy];
    PC += 2;
}
void Chip8::AND(uint8_t Vx, uint8_t Vy){
    //Bitwise AND
    V[Vx] = V[Vx] & V[Vy];
    PC += 2;
}
void Chip8::XOR(uint8_t Vx, uint8_t Vy){
    //Bitwise XOR
    V[Vx] = V[Vx] ^ V[Vy];
    PC += 2;
}
void Chip8::ADD2(uint8_t Vx, uint8_t Vy){
    //Add, check for overflow, set VF flag
    if((uint16_t)V[Vx] + (uint16_t) V[Vy] > UINT8_MAX){
        V[0xF] = 1;
    } else {
        V[0xF] = 0;
    }
    V[Vx] = V[Vx] + V[Vy];
    PC += 2;
}
void Chip8::SUB(uint8_t Vx, uint8_t Vy){
    //Subtract V[x]-V[y], check for underflow, set VF flag
    if(V[Vx] > V[Vy]) {
        V[0xF] = 1;
    } else {
        V[0xF] = 0;
    }
    V[Vx] = V[Vx] - V[Vy];
    PC += 2;
}
void Chip8::SHR(uint8_t Vx){
    uint8_t LSB = V[Vx] & 1;
    V[0xF] = LSB;
    V[Vx] = V[Vx] >> 1; 
    PC += 2; 
}
void Chip8::SUBN(uint8_t Vx, uint8_t Vy){
    //Subtract V[y]-V[x], check for underflow, set VF flag
    if(V[Vy] > V[Vx]) {
        V[0xF] = 1;
    } else {
        V[0xF] = 0;
    }
    V[Vx] = V[Vy] - V[Vx];
    PC += 2;
}


void Chip8::SHL(uint8_t Vx){
    uint8_t MSB = V[Vx] & (1<<7);
    V[0xF] = MSB;
    V[Vx] = V[Vx] >> 1; 
    PC += 2; 
}

void Chip8::SNE2(uint8_t Vx, uint8_t Vy){
    //Skip if not equal
    if(V[Vx] != V[Vy]){
        //Increment the program counter twice to skip
        PC += 4;
    } else {
        PC += 2;
    }
}
void Chip8::LD3(uint16_t addr){
    //Load address into 16-bit I register
    I = addr;
    PC += 2;
}
void Chip8::JP2(uint16_t addr){
    //Jump with offset
    PC = addr + V[0];
}
void Chip8::RND(uint8_t Vx, uint8_t byte) {
    //Set V[Vx] to random byte, bitwise AND with byte argument.
    uint8_t r = (uint8_t)(rand() % 255);
    V[Vx] = r & byte;
    PC += 2; 
}

void Chip8::DRW (uint8_t Vx, uint8_t Vy, uint8_t n){
/*
    Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
The interpreter reads n bytes from memory, starting at the address stored in I. These bytes are then displayed as sprites on screen at coordinates (Vx, Vy). Sprites are XORed onto the existing screen. If this causes any pixels to be erased, VF is set to 1, otherwise it is set to 0. If the sprite is positioned so part of it is outside the coordinates of the display, it wraps around to the opposite side of the screen. See instruction 8xy3 for more information on XOR, and section 2.4, Display, for more information on the Chip-8 screen and sprites.
*/
    if(n > 15){
        printf("ERROR: Sprite size > 15 bytes requested \n");
        PC += 2;
        return;
    } 
    if((uint32_t) I + n > 4095){
        printf("ERROR: DRW Memory Out of Bounds \n");
        PC += 2;
        return;
    }
    uint8_t sprite[n];
    for(int i = 0; i < n; i++){
        sprite[i] = memory[I + i];
    }
    uint8_t start_col = V[Vx];
    uint8_t start_row = V[Vy];

    bool pixel_erased = false;

    //Draw the sprite to the screen
    for(uint8_t row = 0; row < n; row++){
        uint8_t pixel_mask = (1<<7);
        for(uint8_t col=0; col<8; col++){
            //Select pixel position, with wrap around
            uint8_t curr_row = (start_row + row) % height;
            uint8_t curr_col = (start_col + col) % width;
            //Get curr pixel value, and update pixel mask
            uint8_t curr_pixel = sprite[row] & pixel_mask; 
            pixel_mask = pixel_mask >> 1;
            //Check for erased pixels
            if(display[curr_row][curr_col] && curr_pixel){
                pixel_erased = true;
            }
            //Draw pixel (xor with original)
            if(curr_pixel != 0){
                display[curr_row][curr_col] = 1^display[curr_row][curr_col]; 
            }
        }
    }
    //Set the flag if pixels were erased.
    if(pixel_erased == true){
        V[0xF] = 1;
    } else {
        V[0xF] = 0;
    }
    PC += 2;
}

void Chip8::SKP(uint8_t Vx){
    if(V[Vx] > 15){
        printf("ERROR: Invalid Keyboard Index \n");
        PC += 2;
    }
    //Skip if keyboard pressed
    if(keyboard[V[Vx]]){
        //Increment the program counter twice to skip
        PC += 4;
    } else {
        PC += 2;
    }
}

void Chip8::SKNP(uint8_t Vx){
    if(V[Vx] > 15){
        printf("ERROR: Invalid Keyboard Index \n");
        PC += 2;
    }
    //Skip if keyboard not pressed
    if(!keyboard[V[Vx]]){
        //Increment the program counter twice to skip
        PC += 4;
    } else {
        PC += 2;
    }
}

void Chip8::LDK(uint8_t Vx){
    //Block until we get keyboard input
    if(V[Vx] > 15){
        printf("ERROR: Invalid Keyboard Index \n");
        PC += 2;
    }
    if(keyboard[V[Vx]]){
        //Only increment program counter when the keyboard is pressed
        PC += 2;
    } 

    //Otherwise stay on this command.
}
void Chip8::LDDT(uint8_t Vx){
    //Set Delay Timer
    DT = V[Vx];
    PC += 2;
}
void Chip8::LDST(uint8_t Vx){
    //Set Sound Timer
    ST = V[Vx];
    PC += 2;
}
void Chip8::ADDI(uint8_t Vx){
    I = I + V[Vx];
    PC += 2;
}

void Chip8::LDF(uint8_t Vx){
    //Load digit memory location into I
    if(V[Vx] > 15){
        printf("Error: Invalid Digit to load");
    }
    I = this->digitLocations[V[Vx]];
    PC += 2;
}

void Chip8::LDB (uint8_t Vx) {
    //Binary to decimal convert V[Vx], put in position I, I+1, I+2.
    memory[I] = (V[Vx] / 100) % 10; 
    memory[I+1] = (V[Vx] / 10) % 10 ;
    memory[I+2] = V[Vx] % 10;
    PC += 2; 
}

void Chip8::LDI1 (uint8_t Vx) {
    //Store registers V0 through Vx starting in location I
    for(int i=0; i<=Vx; i++){
        memory[I+i] = V[i];
    }
    PC += 2;
}

void Chip8::LDI2 (uint8_t Vx) {
    //Read starting at location I into registers V0 to Vx.
    for(int i=0; i<=Vx; i++){
        V[i] = memory[I+i];
    }
    PC += 2;
}

void Chip8::runInstruction(uint16_t instr) {
    uint8_t word0 = (uint8_t)((instr >> 12) & 0xF);
    uint8_t word1 = (uint8_t)((instr >> 8) & 0xF);
    uint8_t word2 = (uint8_t)((instr >> 4) & 0xF);
    uint8_t word3 = (uint8_t)(instr & 0xF);
    
    uint16_t nnn = instr & 0xFFF;
    uint8_t kk = (uint8_t)(instr & 0xFF);
    uint8_t x = word1;
    uint8_t y = word2;
    

    switch(word0) {
        case 0:
            if(instr == 0x00E0){
                printf(" CLS \n");
                this->CLS();
            } else if(instr == 0x00EE){
                printf(" RET \n");
                this->RET();
            } else {
                printf("\n");
            }
            break;
        case 1:
            printf(" JP %X%X%X \n", word1, word2, word3);
            this->JP1(nnn);
            break;
        case 2:
            printf(" CALL %X%X%X \n", word1, word2, word3); 
            this->CALL(nnn);
            break;
        case 3:
            printf(" SE V%X, %X%X \n", word1, word2, word3);
            this->SE1(x, kk);
            break;
        case 4:
            printf(" SNE V%X, %X%X \n", word1, word2, word3);
            this->SNE1(x, kk);
            break;
        case 5:
            printf(" SE V%X, V%X \n", word1, word2);
            this->SE2(x, y);
            break;
        case 6:
            printf(" LD V%X,  %X%X \n", word1, word2, word3);
            this->LD1(x, kk);
            break;
        case 7: 
            printf( " ADD V%X, %X%X \n", word1, word2, word3);
            this->ADD1(x, kk);
            break;
        case 8:
            switch(word3) {
                case 0:
                    printf( " LD V%X, V%X \n", word1, word2);
                    this->LD2(x, y);
                    break;
                case 1:
                    printf( " OR V%X, V%X \n", word1, word2);
                    this->OR(x, y);
                    break;
                case 2:
                    printf( " AND V%X, V%X \n", word1, word2);
                    this->AND(x, y);
                    break;
                case 3:
                    printf( " XOR V%X, V%X \n", word1, word2);
                    this->XOR(x, y);
                    break;
                case 4:
                    printf( " ADD V%X, V%X \n", word1, word2);
                    this->ADD2(x, y);
                    break;
                case 5:
                    printf( " SUB V%X, V%X \n", word1, word2);
                    this->SUB(x, y);
                    break;
                case 6:
                    printf( " SHR V%X, { V%X } \n", word1, word2);
                    this->SHR(x);
                    break;
                case 7:
                    printf( " SUBN V%X, V%X \n", word1, word2);
                    this->SUBN(x, y);
                    break;
                case 0xE:
                    printf( " SHL V%X, { V%X } \n", word1, word2);
                    this->SHL(x);
                    break;
                default:
                    printf(" %X \n", word0);
            }
            break;
        case 9:
            printf(" SNE V%X, V%X, \n", word1, word2);
            this->SNE2(x, y);
            break;
        case 0xA:
            printf(" LD I, %X%X%X \n", word1, word2, word3);
            this->LD3(nnn);
            break;
        case 0xB:
            printf(" JP V0, %X%X%X \n", word1, word2, word3);
            this->JP2(nnn);
            break;
        case 0xC:
            printf(" RND V%X, %X%X \n", word1, word2, word3);
            this->RND(x, kk);
            break;
        case 0xD:
            printf(" DRW V%X, V%X, %X \n", word1, word2, word3);
            this->DRW(x, y, word3);
            break;
        case 0xE:
            if(word2 == 9 && word3 == 0xE){
                printf(" SKP V%X \n", word1);
                this->SKP(x);
            } else if(word2 == 0xA && word3 == 0x1){
                printf(" SKNP V%X \n", word1);
                this->SKNP(x);
            } else {
                printf(" %X \n", word0);
            }
            break;
        case 0xF:
            switch(kk){
                case 0x07:
                    printf(" LD V%X, DT \n", word1);
                    this->LDDT(x);
                    break;
                case 0x0A:
                    printf(" LD V%X, K \n", word1);
                    this->LDK(x);
                    break;
                case 0x15:
                    printf(" LD DT, V%X \n", word1);
                    this->LDDT(x);
                    break;
                case 0x18:
                    printf(" LD ST, V%X \n", word1);
                    this->LDST(x);
                    break;
                case 0x1E:
                    printf(" ADD I, V%X \n", word1);
                    this->ADDI(x);
                    break;
                case 0x29:
                    printf(" LD F, V%X \n", word1);
                    this->LDF(x);
                    break;
                case 0x33:
                    printf(" LD B, V%X \n", word1);
                    this->LDB(x);
                    break;
                case 0x55:
                    printf(" LD V%X, [I] \n", word1);
                    this->LDI1(x);
                    break;
                case 0x65:
                    printf(" LD [I], V%X \n", word1);
                    this->LDI2(x);
                    break;
                default:
                    printf(" %X \n", word0);
            }
            break;
        default:
            printf(" %X \n", word0);
            break;
    }
}

    
void Chip8::printRom(){
    printf("Printing Instructions \n");        
    FILE* fp = fopen(m_fileName.c_str(), "r");
    if(fp == NULL){
        printf("Unable to read rom! \n");
        return;
    } else {
        unsigned short addr = 0x200;
        while(true) {
            int ch1 = fgetc(fp);
            int ch2 = fgetc(fp);
            if(ch1 == EOF || ch2 == EOF){
                break;
            }
            uint16_t instr = (uint8_t) ch2 | ((uint8_t) ch1 << 8);
            printInstruction(addr, instr);
            addr += 2;
        }
    }
}

void Chip8::printInstruction(uint16_t addr, uint16_t instr) { 
        printf("0x%.4X %.4X", addr, instr);
        //Break the instruction up into 4 words
        uint8_t word0 = (uint8_t)((instr >> 12) & 0xF);
        uint8_t word1 = (uint8_t)((instr >> 8) & 0xF);
        uint8_t word2 = (uint8_t)((instr >> 4) & 0xF);
        uint8_t word3 = (uint8_t)(instr & 0xF);
        uint8_t low_byte = word2 << 4 | word3;
 
        switch(word0) {
            case 0:
                if(instr == 0x00E0){
                    printf(" CLS \n");
                } else if(instr == 0x00EE){
                    printf(" RET \n");
                } else {
                    printf("\n");
                }
                break;
            case 1:
                printf(" JP %X%X%X \n", word1, word2, word3);
                break;
            case 2:
                printf(" CALL %X%X%X \n", word1, word2, word3); 
                break;
            case 3:
                printf(" SE V%X, %X%X \n", word1, word2, word3);
                break;
            case 4:
                printf(" SNE V%X, %X%X \n", word1, word2, word3);
                break;
            case 5:
                printf(" SE V%X, V%X \n", word1, word2);
                break;
            case 6:
                printf(" LD V%X,  %X%X \n", word1, word2, word3);
                break;
            case 7: 
                printf( " ADD V%X, %X%X \n", word1, word2, word3);
                break;
            case 8:
                switch(word3) {
                    case 0:
                        printf( " LD V%X, V%X \n", word1, word2);
                        break;
                    case 1:
                        printf( " OR V%X, V%X \n", word1, word2);
                        break;
                    case 2:
                        printf( " AND V%X, V%X \n", word1, word2);
                        break;
                    case 3:
                        printf( " XOR V%X, V%X \n", word1, word2);
                        break;
                    case 4:
                        printf( " ADD V%X, V%X \n", word1, word2);
                        break;
                    case 5:
                        printf( " SUB V%X, V%X \n", word1, word2);
                        break;
                    case 6:
                        printf( " SHR V%X, { V%X } \n", word1, word2);
                        break;
                    case 7:
                        printf( " SUBN V%X, V%X \n", word1, word2);
                        break;
                    case 0xE:
                        printf( " SHL V%X, { V%X } \n", word1, word2);
                        break;
                    default:
                        printf(" %X \n", word0);
                }
                break;
            case 9:
                printf(" SNE V%X, V%X, \n", word1, word2);
                break;
            case 0xA:
                printf(" LD I, %X%X%X \n", word1, word2, word3);
                break;
            case 0xB:
                printf(" JP V0, %X%X%X \n", word1, word2, word3);
                break;
            case 0xC:
                printf(" RND V%X, %X%X \n", word1, word2, word3);
                break;
            case 0xD:
                printf(" DRW V%X, V%X, %X \n", word1, word2, word3);
                break;
            case 0xE:
                if(word2 == 9 && word3 == 0xE){
                    printf(" SKP V%X \n", word1);
                } else if(word2 == 0xA && word3 == 0x1){
                    printf(" SKNP V%X \n", word1);
                } else {
                    printf(" %X \n", word0);
                }
                break;
            case 0xF:
                switch(low_byte){
                    case 0x07:
                        printf(" LD V%X, DT \n", word1);
                        break;
                    case 0x0A:
                        printf(" LD V%X, K \n", word1);
                        break;
                    case 0x15:
                        printf(" LD DT, V%X \n", word1);
                        break;
                    case 0x18:
                        printf(" LD ST, V%X \n", word1);
                        break;
                    case 0x1E:
                        printf(" ADD I, V%X \n", word1);
                        break;
                    case 0x29:
                        printf(" LD F, V%X \n", word1);
                        break;
                    case 0x33:
                        printf(" LD B, V%X \n", word1);
                        break;
                    case 0x55:
                        printf(" LD V%X, [I] \n", word1);
                        break;
                    case 0x65:
                        printf(" LD [I], V%X \n", word1);
                        break;
                    default:
                        printf(" %X \n", word0);
                }
                break;
            default:
                printf(" %X \n", word0);
                break;
        }
    }

