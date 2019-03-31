#include "chip8.hpp"
#include <SFML/Graphics.hpp>
#include <string>
#include <stdio.h>
#include <ctime>
#include <thread> 
#include <chrono> 
#include <SFML/Audio.hpp>
#include <cmath>
#include "Eigen/Dense"
#include <array>

class PongRL
{
public:
    Chip8* chip8;

    sf::RenderWindow* window;
    
    std::string fileName;
    
    uint32_t pongPlayerScore = 0;
    uint32_t pongOpponentScore = 0;

    const int displayWidth = 640;
    const int displayHeight = 320;
    double clockSpeed = 100.0; //Clock speed in Hz 
    double timerSpeed = 60.0; //Speed in Hz at which timer registers (DT & ST) decrement
    int timer_ms = (int)((1/timerSpeed)*1000);
    sf::SoundBuffer Buffer;
    sf::Sound Sound;
    bool display;
    bool load;
    std::string loadFile;
    PongRL(bool display, bool load, std::string loadFile);    
    
    
    void updateDisplay();
    void stepGame(std::array<bool,16> keyboardInput, int cycleCount, int cyclesPerClockTick); 

private:
    uint16_t dispPlayerScore;
    uint16_t dispOpponentScore;

};


