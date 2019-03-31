#include "rl_pong.hpp"
#include "Eigen/Dense"
#include <string>
#include <array>
#include <unistd.h>
#include <stdint.h>


int main(int argc, char** argv)
{
    printf("argc: %d \n ", argc);

    bool display = false;
    bool load = false;
    std::string loadFile = "";
    bool badArgs = false;

    if(argc > 1 && strncmp(argv[1], "-d", 2) == 0){
        display = true;
        if(argc > 2 && argv[2] == "-l"){
            if(argc == 4){
                loadFile = std::string(argv[3]);
            } else {
                badArgs = true;
            }
        }
    
    } else if(argc > 1 && strncmp(argv[1], "-l", 2) == 0){ 
        if(argc == 3){
            loadFile = std::string(argv[2]);
        } else {
            badArgs = true;
        }
    } else if(argc > 1){
        badArgs = true;
    }
    

    if(badArgs){
        printf("Usage ./rl_pong {-d} {-l loadFile} \n");
        return 0;
    } 
    PongRL pongRL(display, load, loadFile); 
   
    std::clock_t start;
    double duration;
 
    while(true){
        start = std::clock();

        std::array<bool, 16> keyboardInput = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; 
        printf("Our Score: %d Their Score: %d \n", pongRL.pongPlayerScore, pongRL.pongOpponentScore);
        
        pongRL.stepGame(keyboardInput, 6, 3);
        while(true){
             duration=( std::clock() - start ) / (double) CLOCKS_PER_SEC;
             if(true || duration > 0.0001){
                break;
              }
             /*if(display && !pongRL.window->isOpen()){
                return 0;
             }*/
        }   
    }

}




    
