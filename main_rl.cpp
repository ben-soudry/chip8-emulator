#include "rl_pong.hpp"
#include "neuralnet.hpp"
#include "Eigen/Dense"
#include "math.h"
#include <string>
#include <array>
#include <unistd.h>
#include <stdint.h>

//Hyperparameters
const constexpr int maxGameScore = 9;
const constexpr int episodesPerSave = 100;
const constexpr int episodesPerSlowdown = 100;

int main(int argc, char** argv)
{
    NeuralNetwork neuralNetwork;

    //neuralNetwork.saveNetwork("init.nn");

    //neuralNetwork.loadNetwork("init.nn");

    std::ofstream logFile;
    logFile.open("net_1e-3.csv");
    

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
        
    int episodeNumber = 0;
    float averagePerformance = 0.0;         
    float averageFrames = 0.0;         

    while(true){             
        //Start new episode     
        std::default_random_engine gen;
        std::uniform_real_distribution<float> action_distribution(0.0, 1.0);

        Eigen::Matrix<float, Eigen::Dynamic, neuralNetwork.inputSize> Xs; //Inputs
        Eigen::Matrix<float, Eigen::Dynamic, neuralNetwork.numHiddenUnits> Hs; //Hidden unit values
        
        Eigen::Matrix<float, Eigen::Dynamic, neuralNetwork.outputSize> grads; //dL/df - gradient of loss function wrt neuralnet w/out sigmoid.
        Eigen::Matrix<float, Eigen::Dynamic, 1> rewards; //rewards
        
    
        int prevPlayerScore = 0;    
        int prevOpponentScore = 0;
        int currFrame = 0;
        /*if(episodeNumber % 1000 == 0){
            std::cout << "W1: " << neuralNetwork.W1;
            std::cout << "W2: " << neuralNetwork.W2;
        }*/

        //Run an episode
        while(pongRL.pongPlayerScore < maxGameScore && pongRL.pongOpponentScore < maxGameScore){
            start = std::clock();
            
            auto output = new Eigen::Matrix<float, neuralNetwork.outputSize, 1>;
            auto networkInput = new Eigen::Matrix<float, neuralNetwork.inputSize, 1>;
            auto hiddenState = new Eigen::Matrix<float, neuralNetwork.numHiddenUnits, 1>;
            
            neuralNetwork.displayFeedforward(*(pongRL.chip8), output, networkInput, hiddenState);
            
            Hs.conservativeResize(currFrame + 1, neuralNetwork.numHiddenUnits);
            Hs.row(currFrame) = (*hiddenState).transpose();
            
             
            Xs.conservativeResize(currFrame + 1, neuralNetwork.inputSize);
            Xs.row(currFrame) = (*networkInput).transpose();
           
            //Xs.push_back(*networkInput);
            //Hs.push_back(*hiddenState);
            
              
            std::array<bool, 16> keyboardInput;

            if(std::isnan((*output)(0, 0))){
                std::cout << "Warning! Network output is NaN" << std::endl;
            }            
            if(episodeNumber % episodesPerSlowdown == 0){
                std::cout << "Neural Net output: " << (*output)(0,0);
            }
            float random_action = action_distribution(gen);
            float y;
    
            if((*output)(0,0) > random_action) {
                y = 1.0;
                if(episodeNumber % episodesPerSlowdown == 0){
                    std::cout << " - UP" << std::endl;
                }
                keyboardInput = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; 
            } else {
                y = 0.0;
                if(episodeNumber % episodesPerSlowdown == 0){
                    std::cout << " - DOWN" << std::endl;
                }
                keyboardInput = {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; 
            }
            Eigen::Matrix<float, 1, neuralNetwork.outputSize> grad; 
            grad(0,0) = y - (*output)(0,0);
            
            grads.conservativeResize(currFrame + 1, neuralNetwork.outputSize);
            grads.row(currFrame) = grad;
                    
            
            pongRL.stepGameFrame(keyboardInput); // 10,5
            
            Eigen::Matrix<float, 1, 1> currReward;
            currReward(0,0) = 0.0;
            if(pongRL.pongPlayerScore > prevPlayerScore){
               //std::cout << "we got reward?" << std::endl;
               currReward(0,0) += 1.0;
            }
            if(pongRL.pongOpponentScore > prevOpponentScore){
               //std::cout << "they got reward?"<< std::endl;
               currReward(0,0) -= 1.0;
            }
        
            rewards.conservativeResize(currFrame + 1, 1); 
            rewards.row(currFrame) = currReward;
            
            currFrame += 1;
            //printf("Our Score: %d Their Score: %d \n", pongRL.pongPlayerScore, pongRL.pongOpponentScore);
            
            prevPlayerScore = pongRL.pongPlayerScore;    
            prevOpponentScore = pongRL.pongOpponentScore;
            
            delete output;
            delete networkInput;
            delete hiddenState;
     

            while(true){
                 duration=( std::clock() - start ) / (double) CLOCKS_PER_SEC;
                 if(!((episodeNumber % episodesPerSlowdown) == 0)  ||  duration > 0.030){
                    break;
                  }
                 /*if(display && !pongRL.window->isOpen()){
                    return 0;
                 }*/
            }   
        }
 
        float currPerformance = (float)pongRL.pongPlayerScore - (float) pongRL.pongOpponentScore;      
        printf("Our Score: %d Their Score: %d Peformance: %f \n", pongRL.pongPlayerScore, pongRL.pongOpponentScore, currPerformance);

        pongRL.resetGame();
        
        neuralNetwork.discountRewards(rewards);

        /*std::cout << "Discounted Episode Rewards: ";
        for(std::vector<float>::iterator it = rewards.begin(); it != rewards.end(); ++it){
            std::cout << *it << ","; 
        }*/
        std::cout << std::endl;

        averagePerformance = (((float) episodeNumber)*averagePerformance + currPerformance)/((float)(episodeNumber + 1));
        
        float currFrames = (float) grads.rows();  
        averageFrames = (((float) episodeNumber)*averageFrames + currFrames)/((float)(episodeNumber + 1));

        std::cout << "Episode " << episodeNumber << " Complete, Average Performance: " << averagePerformance << " | Average Frames: "<< averageFrames << std::endl;
        
        std::cout << "=========================================================" << std::endl;
        
        logFile << episodeNumber << ", " << averagePerformance << ", " << averageFrames << std::endl;

        if(episodeNumber % episodesPerSave == 0){
            neuralNetwork.saveNetwork("net_1e-3_ep" + std::to_string(episodeNumber) + ".nn"); 
        }

        episodeNumber += 1;
 
        //Backprop step:
        neuralNetwork.backprop(Xs, Hs, grads, rewards); 
        
    }
}
    
