#include "neuralnet.hpp"

NeuralNetwork::NeuralNetwork()
{

}

NeuralNetwork::initializeWeights()
{
    //Initialize w/standard normal
}

NeuralNetwork::feedforward(Matrix<float, inputSize, 1>& X, Matrix<float, outputSize,1>* Y)
{

    //Add sigmoid activations
    *Y = W2*W1*X;
    return;
}

