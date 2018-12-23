#include <stdint.h>
#include <stdlib.h>
#include <string.h>  
#include <string>
#include <mutex>

class Chip8
{
public:
    Chip8(std::string fileName);
    void loadROM();
    std::string m_fileName;
    //Program memory, program counter and stack.
    uint8_t memory[4096];
    uint16_t PC;
    uint16_t stack[16]; 
    int stack_ptr; //top of stack - -1 means empty stack
     
    //Registers (16 8-bit and 1 16-bit)
    uint8_t V[16];
    uint16_t I;
    //Special registers (Delay Timer and Sound Timer)
    std::mutex DT_ST_Mutex;  //Allows safe asyncronous reads/writes to DT and ST
    uint8_t DT;
    uint8_t ST;
    
    static const uint8_t width = 64;
    static const uint8_t height = 32;

    bool display[height][width];
    
    bool keyboard[16];    
    
    
    //When set, the screen has been updated 
    bool draw_flag;
    
    
    void setKey(uint8_t key, bool press);
    void tickClock(); 
    bool isSoundOn();
    void emulateCycle(); 
    void runInstruction(uint16_t instr);
 
    void printRom();
    void printInstruction(uint16_t addr, uint16_t instr);

    //Memory locations of digits 0-F
    static constexpr uint16_t digitLocations[16] = {0,5,10,15,20,25,30,35,40,45,50,55,60,65,70,75};
    
    static constexpr uint8_t digits[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
    

private:
    void initRegisters();
    void clearDisplay();
    
    //Chip8 instruction implementations
    //See Chip8 documentation for details:
    //http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#2.4
    void CLS();
    void RET();
    void JP1 (uint16_t addr);
    void CALL(uint16_t addr);
    void SE1 (uint8_t Vx, uint8_t byte);
    void SNE1(uint8_t Vx, uint8_t byte);
    void SE2 (uint8_t Vx, uint8_t Vy);
    void LD1 (uint8_t Vx, uint8_t byte);
    void ADD1(uint8_t Vx, uint8_t byte);
    void LD2 (uint8_t Vx, uint8_t Vy);
    void OR  (uint8_t Vx, uint8_t Vy);
    void AND (uint8_t Vx, uint8_t Vy);
    void XOR (uint8_t Vx, uint8_t Vy);
    void ADD2(uint8_t Vx, uint8_t Vy);
    void SUB (uint8_t Vx, uint8_t Vy);
    void SHR (uint8_t Vx);
    void SUBN(uint8_t Vx, uint8_t Vy);
    void SHL (uint8_t Vx);
    void SNE2(uint8_t Vx, uint8_t Vy);
    void LD3 (uint16_t addr);
    void JP2 (uint16_t addr);
    void RND (uint8_t Vx, uint8_t byte);
    void DRW (uint8_t Vx, uint8_t Vy, uint8_t n);
    void SKP (uint8_t Vx);
    void SKNP(uint8_t Vx);
    void LDDT(uint8_t Vx);
    void LDDT2(uint8_t Vx);
    void LDK (uint8_t Vx);
    void LDST(uint8_t Vx);
    void ADDI(uint8_t Vx);
    void LDF (uint8_t Vx);
    void LDB (uint8_t Vx);
    void LDI1(uint8_t Vx);
    void LDI2(uint8_t Vx);
 };


