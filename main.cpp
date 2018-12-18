#include <SFML/Graphics.hpp>
#include <string>
#include <stdio.h>
#include "chip8.hpp"
#include <time.h>

const int displayWidth = 640;
const int displayHeight = 320;

void getKeyboardInput(sf::RenderWindow& window, Chip8 chip8){
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed){
            window.close();
        }
    }
}

void updateDisplay(sf::RenderWindow& window, Chip8& chip8){

    window.clear();

    float pixelWidth = (float) displayWidth / (float) chip8.width;
    float pixelHeight = (float) displayHeight / (float) chip8.height;
    bool noPixels = true; 
    for(int row = 0; row < chip8.height; row++){
        for(int col = 0; col < chip8.width; col++){
            if(chip8.display[row][col]){
                noPixels = false;
                sf::RectangleShape pixel(sf::Vector2f(pixelWidth, pixelHeight));
                pixel.setFillColor(sf::Color::Green);
                pixel.setPosition(col*pixelWidth, row*pixelHeight);                
                window.draw(pixel);
            }
        }
    }
    chip8.draw_flag = 0;
    window.display();
}


int main()
        
{
    //Chip8 chip8("roms/games/Airplane.ch8");
    
    Chip8 chip8("roms/games/Space\ Invaders\ \[David\ Winter\].ch8");
    chip8.printRom();

    sf::RenderWindow window(sf::VideoMode(displayWidth, displayHeight), "Chip8 Display");
    //window.setFramerateLimit(60);
    //window.setVerticalSyncEnabled(true);
  
    struct timespec start, finish;
    double elapsed;



    while (window.isOpen())
    {
        //Get user input:
        getKeyboardInput(window, chip8);

        //Emulate cycle:
        do {
            chip8.emulateCycle();
        } while(chip8.draw_flag == 0);

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
    
