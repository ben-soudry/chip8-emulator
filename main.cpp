#include <SFML/Graphics.hpp>
#include <string>
#include <stdio.h>
#include "chip8.hpp"
#include <ctime>

const int displayWidth = 640;
const int displayHeight = 320;
double clockSpeed = 100.0; //Clock speed in Hz 


void getKeyboardInput(sf::RenderWindow& window, Chip8& chip8){
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed){
            window.close();
        }
    }
    //Keyboard mapping:
    chip8.setKey(0x1, sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad1));
    chip8.setKey(0x2, sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad2));
    chip8.setKey(0x3, sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad3));
    chip8.setKey(0xC, sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad4));
    chip8.setKey(0x4, sf::Keyboard::isKeyPressed(sf::Keyboard::Q));
    chip8.setKey(0x5, sf::Keyboard::isKeyPressed(sf::Keyboard::W));
    chip8.setKey(0x6, sf::Keyboard::isKeyPressed(sf::Keyboard::E));
    chip8.setKey(0xD, sf::Keyboard::isKeyPressed(sf::Keyboard::R));
    chip8.setKey(0x7, sf::Keyboard::isKeyPressed(sf::Keyboard::A));
    chip8.setKey(0x8, sf::Keyboard::isKeyPressed(sf::Keyboard::S));
    chip8.setKey(0x9, sf::Keyboard::isKeyPressed(sf::Keyboard::D));
    chip8.setKey(0xE, sf::Keyboard::isKeyPressed(sf::Keyboard::F));
    chip8.setKey(0xA, sf::Keyboard::isKeyPressed(sf::Keyboard::Z));
    chip8.setKey(0x0, sf::Keyboard::isKeyPressed(sf::Keyboard::X));
    chip8.setKey(0xB, sf::Keyboard::isKeyPressed(sf::Keyboard::C));
    chip8.setKey(0xF, sf::Keyboard::isKeyPressed(sf::Keyboard::V));

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

int main(int argc, char** argv)
{
    if(argc != 3){
        printf("Usage ./chip8-app <ROM File> <Clock Speed (Hz)> \n");
        return 0;
    } 
    std::string rom_name(argv[1]);
    clockSpeed = atof(argv[2]);

    Chip8 chip8(rom_name);
    
    printf("Printing ROM.... \n");
    chip8.printRom();
    printf("End of ROM print \n");

    sf::RenderWindow window(sf::VideoMode(displayWidth, displayHeight), "Chip8 Display");
       
    //window.setFramerateLimit(60);
    //window.setVerticalSyncEnabled(true);
  
    std::clock_t start;
    double duration;

    while (window.isOpen())
    {
        do {
            //Emulate cycle:
            start = std::clock();

            //Get user input:
            getKeyboardInput(window, chip8);        
            
            chip8.emulateCycle();
            
            while(true){
                duration=( std::clock() - start ) / (double) CLOCKS_PER_SEC;
                if(duration > 1/(clockSpeed)){
                    break;
                }
            }
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
    
