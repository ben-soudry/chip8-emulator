#include "rl_pong.hpp"
#include <stdio.h>
PongRL::PongRL(bool display) : display(display)
{
    std::string romName = "roms/games/Pong\ \(1\ player\).ch8";

    this->chip8 = new Chip8(fileName);

    if(display){
        char title[200];
        snprintf(title, 200, "Chip8: %s", fileName.c_str());
        this->window = new sf::RenderWindow(sf::VideoMode(displayWidth, displayHeight), title);
    }  
}

void PongRL::stepGameFrame(std::array<bool,16> keyboardInput){
    sf::Event event;
    while (display && window->pollEvent(event))
    {
        if (event.type == sf::Event::Closed){
            window->close();
            display = false;
        }
    }
    
    applyKeyboardInput(keyboardInput);
    
    int cycle = 0;
    do {
        chip8->emulateCycle();

        chip8->tickClock();

        //Pong score capture code
        if(chip8->PC == 0x2DC){
            //printf("Our Score Updated!");
            uint16_t oldScore = dispPlayerScore;
            dispPlayerScore  = chip8->I / 5;
            if(oldScore != dispPlayerScore && oldScore >= 0 && oldScore < 10){
                //printf("ours was: %d is: %d \n", oldScore, dispPlayerScore);
                pongPlayerScore += 1;
            }
        } 
        if(chip8->PC == 0x2E6){
            //printf("Their Score Updated");
            uint16_t oldScore = dispOpponentScore;
            dispOpponentScore = chip8->I / 5;
            if(oldScore != dispOpponentScore && oldScore >= 0 && oldScore < 10){
                //printf("Theirs was: %d is: %d \n", oldScore, dispOpponentScore);
                pongOpponentScore += 1;
            }
        }
        cycle++;
    } while(chip8->PC != 0x254);
        
    //printf("Num Cycles in Frame: %d \n", cycle);

    if(display && chip8->draw_flag == 1){
        updateDisplay();
        chip8->draw_flag = 0;
    }
}

void PongRL::updateDisplay(){
    //printf("updating display \n");

    window->clear();
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
                window->draw(pixel);
            }
        }
    }
    chip8->draw_flag = 0;
    window->display();
}

void PongRL::resetGame(){
   fileName = "roms/games/Pong\ \(1\ player\).ch8";

   delete chip8;
   this->chip8 = new Chip8(fileName);

 
    pongPlayerScore = 0;
    pongOpponentScore = 0;
    
    dispPlayerScore = 0;
    dispOpponentScore = 0;

}


void PongRL::applyKeyboardInput(std::array<bool,16> keyboardInput){
    chip8->setKey(0x1, keyboardInput[0]);
    chip8->setKey(0x2, keyboardInput[1]);
    chip8->setKey(0x3, keyboardInput[2]);
    chip8->setKey(0xC, keyboardInput[3]);
    chip8->setKey(0x4, keyboardInput[4]);
    chip8->setKey(0x5, keyboardInput[5]);
    chip8->setKey(0x6, keyboardInput[6]);
    chip8->setKey(0xD, keyboardInput[7]);
    chip8->setKey(0x7, keyboardInput[8]);
    chip8->setKey(0x8, keyboardInput[9]);
    chip8->setKey(0x9, keyboardInput[10]);
    chip8->setKey(0xE, keyboardInput[11]);
    chip8->setKey(0xA, keyboardInput[12]);
    chip8->setKey(0x0, keyboardInput[13]);
    chip8->setKey(0xB, keyboardInput[14]);
    chip8->setKey(0xF, keyboardInput[15]);
}
