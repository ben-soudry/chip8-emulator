#include "rl_pong.hpp"
#include "Eigen/Dense"
#include <string>
#include <array>
#include <unistd.h>
#include <stdint.h>


class NeuralNetwork
{
public: 
  constexpr const int inputSize = 64*32;
  constexpr const int numHiddenUnits = 200;
  constexpr const int outputSize = 1;


  Matrix<float, numHiddenUnits , inputSize> W1;
  Matrix<float, numHiddenUnits , outputSize> W2;
  
  
  NeuralNetwork();
  initializeWeights();
  feedforward(Matrix<float, inputSize , 1>& X,
              Matrix<float, outputSize , 1>* Y);
    
private:
  

};
