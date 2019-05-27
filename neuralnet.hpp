#include "Eigen/Dense"
#include "chip8.hpp"
#include <string>
#include <array>
#include <unistd.h>
#include <stdint.h>
#include <math.h>  
#include <vector>  
#include <iostream>
#include <fstream>
#include <sstream>


class NeuralNetwork
{
public: 
  //Hyperparameters
  static const constexpr int inputSize = 64*32;
  static const constexpr int numHiddenUnits = 400;
  static const constexpr int outputSize = 1;

  float learningRate; //1e-3
  float discountFactor; //0.995

  Eigen::MatrixXf W1;  //(numHiddenUnits , inputSize);
  Eigen::MatrixXf W2;  //(outputSize, numHiddenUnits); //<float, numHiddenUnits , outputSize> W2;
  
  bool lastDisplay[32][64];
 
  NeuralNetwork();
  void initializeWeights();
  void feedForwardDisplay(bool display);
  void displayFeedforward(Chip8& chip8, Eigen::Matrix<float, outputSize,1>* output,
                          Eigen::Matrix<float, inputSize,1>* networkInput,
                          Eigen::Matrix<float, numHiddenUnits, 1>* hiddenState);
   
  void feedforward(Eigen::Matrix<float, inputSize , 1>& X,
                   Eigen::Matrix<float, outputSize , 1>* Y,
                   Eigen::Matrix<float, numHiddenUnits, 1>* hiddenState);



  void discountRewards(Eigen::Matrix<float, Eigen::Dynamic, 1>& rewards);

   
  void backprop( 
        Eigen::Matrix<float, Eigen::Dynamic, inputSize> Xs,
        Eigen::Matrix<float, Eigen::Dynamic, numHiddenUnits> Hs,
        Eigen::Matrix<float, Eigen::Dynamic, outputSize> grads,
        Eigen::Matrix<float, Eigen::Dynamic, 1> rewards);
    
  void backprop_fast( 
        Eigen::Matrix<float, Eigen::Dynamic, inputSize> Xs,
        Eigen::Matrix<float, Eigen::Dynamic, numHiddenUnits> Hs,
        Eigen::Matrix<float, Eigen::Dynamic, outputSize> grads,
        Eigen::Matrix<float, Eigen::Dynamic, 1> rewards);
   
  void backprop_slow( 
        Eigen::Matrix<float, Eigen::Dynamic, inputSize> Xs,
        Eigen::Matrix<float, Eigen::Dynamic, numHiddenUnits> Hs,
        Eigen::Matrix<float, Eigen::Dynamic, outputSize> grads,
        Eigen::Matrix<float, Eigen::Dynamic, 1> rewards);
  
  void saveNetwork(std::string filename);   
  void loadNetwork(std::string filename);
  

private:
  static float sigmoid(float x); 
  static float relu(float x); 

  static float sigmoidDeriv(float x);
  static float reluDeriv(float x);
    
  static float standardNormal(float x);

  std::default_random_engine gen;
  std::normal_distribution<float> W1_normalDist;
  std::normal_distribution<float> W2_normalDist;

};



