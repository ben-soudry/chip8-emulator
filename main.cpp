#include "chip8.hpp"
#include <SFML/Graphics.hpp>
#include <string>
#include <stdio.h>
#include <ctime>
#include <thread> 
#include <chrono> 
#include <SFML/Audio.hpp>
#include <cmath>


const int displayWidth = 1280;
const int displayHeight = 640;
double clockSpeed = 100.0; //Clock speed in Hz 
double timerSpeed = 60.0; //Speed in Hz at which timer registers (DT & ST) decrement
int timer_ms = (int)((1/timerSpeed)*1000);
sf::SoundBuffer Buffer;
sf::Sound Sound;

void getKeyboardInput(sf::RenderWindow& window, Chip8* chip8){
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed){
            window.close();
        }
    }
    //Keyboard mapping:
    chip8->setKey(0x1, sf::Keyboard::isKeyPressed(sf::Keyboard::Num1));
    chip8->setKey(0x2, sf::Keyboard::isKeyPressed(sf::Keyboard::Num2));
    chip8->setKey(0x3, sf::Keyboard::isKeyPressed(sf::Keyboard::Num3));
    chip8->setKey(0xC, sf::Keyboard::isKeyPressed(sf::Keyboard::Num4));
    chip8->setKey(0x4, sf::Keyboard::isKeyPressed(sf::Keyboard::Q));
    chip8->setKey(0x5, sf::Keyboard::isKeyPressed(sf::Keyboard::W));
    chip8->setKey(0x6, sf::Keyboard::isKeyPressed(sf::Keyboard::E));
    chip8->setKey(0xD, sf::Keyboard::isKeyPressed(sf::Keyboard::R));
    chip8->setKey(0x7, sf::Keyboard::isKeyPressed(sf::Keyboard::A));
    chip8->setKey(0x8, sf::Keyboard::isKeyPressed(sf::Keyboard::S));
    chip8->setKey(0x9, sf::Keyboard::isKeyPressed(sf::Keyboard::D));
    chip8->setKey(0xE, sf::Keyboard::isKeyPressed(sf::Keyboard::F));
    chip8->setKey(0xA, sf::Keyboard::isKeyPressed(sf::Keyboard::Z));
    chip8->setKey(0x0, sf::Keyboard::isKeyPressed(sf::Keyboard::X));
    chip8->setKey(0xB, sf::Keyboard::isKeyPressed(sf::Keyboard::C));
    chip8->setKey(0xF, sf::Keyboard::isKeyPressed(sf::Keyboard::V));

}

void updateDisplay(sf::RenderWindow& window, Chip8* chip8){
    window.clear();

    float pixelWidth = (float) displayWidth / (float) chip8->width;
    float pixelHeight = (float) displayHeight / (float) chip8->height;
    bool noPixels = true; 
    for(int row = 0; row < chip8->height; row++){
        for(int col = 0; col < chip8->width; col++){
            if(chip8->display[row][col]){
                noPixels = false;
                sf::RectangleShape pixel(sf::Vector2f(pixelWidth, pixelHeight));
                pixel.setFillColor(sf::Color::Green);
                pixel.setPosition(col*pixelWidth, row*pixelHeight);                
                window.draw(pixel);
            }
        }
    }
    chip8->draw_flag = 0;
    window.display();
}

void timerRegistersManager(Chip8* chip8) 
{
    while(true){
        chip8->tickClock();
        printf("TICK! \n");
        std::this_thread::sleep_for(std::chrono::milliseconds(timer_ms));      
    } 
    
}

void initSound(){
    //Sound code adapted from https://github.com/SFML/SFML/wiki/Tutorial:-Play-Sine-Wave
    const unsigned SAMPLES = 44100;
    const unsigned SAMPLE_RATE = 44100;
    
    sf::Int16 raw[SAMPLES];
    
    const double AMPLITUDE = 30000;
    const double TWO_PI = 6.28318;
    const double increment = 440.0/SAMPLES;
    double x = 0;

    for(unsigned i = 0; i < SAMPLES; i++) {
        raw[i] = AMPLITUDE * sin(x*TWO_PI);
        x += increment;
    }

    if(!Buffer.loadFromSamples(raw, SAMPLES, 1, SAMPLE_RATE)){
        printf("Failed to load audio \n");
    }
    
    Sound.setBuffer(Buffer);
    Sound.setLoop(true);
    
    Sound.play(); 
    sf::sleep(sf::milliseconds(10));
    Sound.stop();
}

int main(int argc, char** argv)
{
    if(argc != 3){
        printf("Usage ./chip8-app <ROM File> <Clock Speed (Hz)> \n");
        return 0;
    } 
    std::string rom_name(argv[1]);
    clockSpeed = atof(argv[2]);

    
    Chip8* chip8 = new Chip8(rom_name);
    
    //Print out ROM
    printf("Printing ROM.... \n");
    chip8->printRom();
    printf("End of ROM print \n");

    //Create Display window with rom title
    char title[200];
    snprintf(title, 200, "Chip8: %s", rom_name.c_str());
    sf::RenderWindow window(sf::VideoMode(displayWidth, displayHeight), title);
       
    //Start up a thread for updating the time registers at 60Hz
    std::thread timerRegistersThread(timerRegistersManager, chip8);    

    //initialize sound object
    initSound();
    bool soundOn = false;   
    

    std::clock_t start;
    double duration;
    
    while (window.isOpen())
    {
        do {
            //Emulate cycle:
            start = std::clock();

            //Get user input:
            getKeyboardInput(window, chip8);        
                
            chip8->emulateCycle();
            if(soundOn == false && chip8->isSoundOn()){
                Sound.play();
                printf("Sound On! \n");
                soundOn = true;
            } else if(soundOn == true && !chip8->isSoundOn()){
                Sound.stop();
                printf("Sound Off! \n");
                soundOn = false;
            }


            while(true){
                duration=( std::clock() - start ) / (double) CLOCKS_PER_SEC;
                if(duration > 1/(clockSpeed)){
                    break;
                }
            }
        } while(chip8->draw_flag == 0);

        /*
        chip8.V[0] = 6;
        chip8.V[3] = 4;
        chip8.runInstruction(0xF329);
        
        //Draw digit:
        chip8.runInstruction(0xD005);
        */

        //draw
        updateDisplay(window, chip8);
    }

    return 0;
}
    
