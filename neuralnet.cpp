#include "neuralnet.hpp"

NeuralNetwork::NeuralNetwork()
{
  learningRate = 1e-3; //1e-3
  discountFactor = 0.995;    

  W1 = Eigen::MatrixXf(numHiddenUnits , inputSize);
  W2 = Eigen::MatrixXf(outputSize, numHiddenUnits);

  W1_normalDist = std::normal_distribution<float>(0.0, 2.0/(inputSize)); 
  W2_normalDist = std::normal_distribution<float>(0.0, 2.0/(numHiddenUnits)); 
    
  for(int i = 0; i < 32; i++){
      for(int j = 0; j < 64; j++){
          lastDisplay[i][j] = 0;
      }
  }
   
  this->initializeWeights();

}

void NeuralNetwork::initializeWeights()
{
    //Initialize w/standard normal
    std::function<float(float)> W1_initializationFunc = std::bind(W1_normalDist, gen);
    W1 = W1.unaryExpr(W1_initializationFunc);
   
    std::function<float(float)> W2_initializationFunc = std::bind(W2_normalDist, gen);
    W2 = W2.unaryExpr(W2_initializationFunc);

}
void NeuralNetwork::displayFeedforward(Chip8& chip8, Eigen::Matrix<float, outputSize,1>* output, 
                                                     Eigen::Matrix<float, inputSize,1>* networkInput,                                                                              
                                                     Eigen::Matrix<float, numHiddenUnits,1>* hiddenState){
    
    Eigen::Matrix<float, inputSize, 1> X;
    int X_index = 0;
    for(int i = 0; i < chip8.height; i++){
        for(int j = 0; j < chip8.width; j++){
            X(X_index, 0) = (float) chip8.display[i][j] - (float) lastDisplay[i][j];
            X_index += 1;
            lastDisplay[i][j] = chip8.display[i][j];
        }
    }
    
    //std::cout << X.transpose() << std::endl;
    *networkInput = X;
    
    this->feedforward(X, output, hiddenState); 
} 
void NeuralNetwork::feedforward(Eigen::Matrix<float, inputSize, 1>& X, Eigen::Matrix<float, outputSize,1>* Y, Eigen::Matrix<float, numHiddenUnits,1>* hiddenState)
{
    std::function<float(float)> activationFunc = relu;
    
    //Apply First layer with relu activation
    Eigen::Matrix<float, numHiddenUnits, 1> hiddenLayer = W1*X;
    hiddenLayer = hiddenLayer.unaryExpr(activationFunc);
    
    //Store hidden layer*
    *hiddenState = hiddenLayer;        

    //Apply Second layer with sigmoid activation
    std::function<float(float)> activationFunc2 = sigmoid;
    
    *Y = W2*hiddenLayer;
    *Y = (*Y).unaryExpr(activationFunc2);
     
   
    return;
}


void NeuralNetwork::backprop(Eigen::Matrix<float, Eigen::Dynamic, inputSize> Xs,
                             Eigen::Matrix<float, Eigen::Dynamic, numHiddenUnits> Hs,
                             Eigen::Matrix<float, Eigen::Dynamic, outputSize> grads,
                             Eigen::Matrix<float, Eigen::Dynamic, 1> rewards){

    backprop_fast(Xs, Hs, grads, rewards);
    //backprop_slow(Xs, Hs, grads, rewards);

}

void NeuralNetwork::backprop_fast(Eigen::Matrix<float, Eigen::Dynamic, inputSize> Xs,
                             Eigen::Matrix<float, Eigen::Dynamic, numHiddenUnits> Hs,
                             Eigen::Matrix<float, Eigen::Dynamic, outputSize> grads,
                             Eigen::Matrix<float, Eigen::Dynamic, 1> rewards){
    std::cout << "Peforming Backprop (Vectorized)" << std::endl;
    double start = std::clock();

    //W2 Backprop Step
    Eigen::Matrix<float, outputSize, numHiddenUnits> W2_GradBuffer = Eigen::MatrixXf::Zero(outputSize, numHiddenUnits);
    
    //std::cout << "Grads (no rewards): " << grads.transpose() << std::endl; 
    grads = grads.cwiseProduct(rewards); 
     
    //std::cout << "Rewards (backprop): " << rewards.transpose() << std::endl; 
 
    //std::cout << "Grads (with rewards): " << grads.transpose() << std::endl; 
    W2_GradBuffer = Hs.transpose()*grads;    
        
    //W1 Backprop Step
 
    Eigen::MatrixXf dH = W2.transpose()*grads.transpose();  //hidden units x frames
    for(int i = 0; i < numHiddenUnits; i++){
        for(int j = 0; j < grads.rows(); j++){
            //std::cout << "Hidden Unit: " << Hs(j, i) << std::endl;
            if(Hs(j, i) <= 0){
                //std::cout << "this ran:"<< Hs(j,i) << std::endl;
                dH(i, j) = 0;
            }
        } 
    }
        
    Eigen::MatrixXf W1_GradBuffer =  dH*Xs;
    W1 = W1 + learningRate*W1_GradBuffer;

    W2 = W2 + learningRate*W2_GradBuffer;

    
    //std::cout << "GradBuf W1: "<< W1_GradBuffer << std::endl;
    //std::cout << "GradBuf W2: "<< W2_GradBuffer << std::endl;
    
    //std::cout << "Backprop W1: "<< W1 << std::endl;
    //std::cout << std::endl;
    //std::cout << "Backprop W2: "<< W2 << std::endl;
    double duration=( std::clock() - start ) / (double) CLOCKS_PER_SEC;
    std::cout << "Backprop completed in " << duration << " seconds" << std::endl;
  

}

void NeuralNetwork::backprop_slow(Eigen::Matrix<float, Eigen::Dynamic, inputSize> Xs,
                             Eigen::Matrix<float, Eigen::Dynamic, numHiddenUnits> Hs,
                             Eigen::Matrix<float, Eigen::Dynamic, outputSize> grads,
                             Eigen::Matrix<float, Eigen::Dynamic, 1> rewards){
    std::cout << "Peforming Backprop (Unvectorized)" << std::endl;
    double start = std::clock();

    //W2 Backprop Step
    Eigen::Matrix<float, outputSize, numHiddenUnits> W2_GradBuffer = Eigen::MatrixXf::Zero(outputSize, numHiddenUnits);

    
    for(int i = 0; i < grads.rows(); i++){
        for(int j = 0; j < numHiddenUnits; j++){   
            
            W2_GradBuffer(0, j) += rewards(i, 0)*Hs(i,j)*grads(i, 0);   
        }
    }

    //W1 Backprop Step
    Eigen::MatrixXf W1_GradBuffer = Eigen::MatrixXf::Zero(numHiddenUnits, inputSize);    
    for(std::vector<float>::size_type i = 0; i < grads.size(); i++){
        for(int k = 0; k < inputSize; k++){
            for(int j = 0; j < numHiddenUnits; j++){   
                W1_GradBuffer(j, k) += rewards(i,0)*reluDeriv(W1(j,k)*Xs(i,k))*Xs(i,k)*W2(0,j)*grads(i, 0);   
            }
        }
    }

    //W1 = W1 + learningRate*W1_GradBuffer;
    
    W2 = W2 + learningRate*W2_GradBuffer;

    //std::cout << "GradBuf W1: "<< W1_GradBuffer << std::endl;
    //std::cout << "GradBuf W2: "<< W2_GradBuffer << std::endl;
 


    //std::cout << "GradBuf : "<< W2_GradBuffer << std::endl;
    //std::cout << "Backprop W2: "<< W2 << std::endl;
    double duration=( std::clock() - start ) / (double) CLOCKS_PER_SEC;
    std::cout << "Backprop completed in " << duration << " seconds" << std::endl;
  
}
 

void NeuralNetwork::discountRewards(Eigen::Matrix<float, Eigen::Dynamic, 1>& rewards){
    float last = 0.0;
    
    //std::cout << "Rewards (Undiscounted): " << rewards.transpose() << std::endl;

    for(int i = rewards.rows()-1; i >= 0; i--){
        if(rewards(i,0) != 0){
            last = 0.0; //Pong-specific - game resets upon scoring, no state is saved.
        }
        rewards(i,0)  += last*discountFactor;
        last = rewards(i,0);
    }
        
    
    //std::cout << "Rewards (Unnormalized): " << rewards.transpose() << std::endl;
    //Normalize rewards
    //float sum = std::accumulate(rewards.begin(), rewards.end(), 0.0);
    //float mean = sum / rewards.size();
    float mean = rewards.mean();    

    //float sq_sum = std::inner_product(rewards.begin(), rewards.end(), rewards.begin(), 0.0);
    //float stdev = std::sqrt(sq_sum / rewards.size() - mean * mean);   
    //float stdev = std::sqrt((rewards - rewards.mean()).square().sum()/(rewards.size()-1));    

    //std::for_each(rewards.begin(), rewards.end(), [mean](float &r){ r -= mean;});
    //std::for_each(rewards.begin(), rewards.end(), [stdev](float &r){ r /= stdev;});
    
    rewards = rewards - Eigen::MatrixXf::Constant(rewards.rows(), 1, mean);
      
    Eigen::MatrixXf squaredRewards = rewards.cwiseProduct(rewards);  //rewards^2    
    float stdev = std::sqrt(squaredRewards.sum()/rewards.rows()/2); 
    
    
    rewards = rewards / stdev;

    //std::cout << "Rewards: " << rewards.transpose() << std::endl;
}



float NeuralNetwork::sigmoid(float x){
    return (1.0 / (1.0 + exp(-x)));
}
float NeuralNetwork::relu(float x){
    return (x > 0) ? x : 0;
}

float NeuralNetwork::sigmoidDeriv(float x){
    return (exp(-x)/((1 + exp(-x))*(1 + exp(-x))));
}

float NeuralNetwork::reluDeriv(float x){
    return (x > 0) ? 1 : 0;
}
float NeuralNetwork::standardNormal(float x){
    return 0;
}

//File I/O - Read/Write Network data to a file

void NeuralNetwork::saveNetwork(std::string filename){
  std::ofstream saveFile;
  saveFile.open(filename);
  saveFile << inputSize << std::endl;
  saveFile << numHiddenUnits << std::endl;
  saveFile << outputSize << std::endl;
  saveFile << learningRate << std::endl;


  saveFile << W1 << std::endl;
  saveFile << std::endl;
  saveFile << W2 << std::endl;


  saveFile.close();
}

void NeuralNetwork::loadNetwork(std::string filename){
    std::ifstream loadFile;
    loadFile.open(filename);
    std::string line;
    
    getline(loadFile, line);
    int fileInputSize = std::atoi(line.c_str());
    
    getline(loadFile, line);
    int fileNumHiddenUnits = std::atoi(line.c_str());

    getline(loadFile, line);
    int fileOutputSize = std::atoi(line.c_str());

    if(inputSize != fileInputSize ||
       numHiddenUnits != fileNumHiddenUnits ||
       outputSize != fileOutputSize){
       std::cout << "Error mismatched network params in load file" << std::endl;
       
       loadFile.close();
       return;
    }
    getline(loadFile, line);
    learningRate  = std::stof(line.c_str());
    
    int row = 0;
    while (std::getline(loadFile, line))
    {   
        std::stringstream iss(line); 
        for(int col = 0; col < W1.cols(); col++){
            std::string val;
            iss >> val;
            W1(row, col) = std::stof(val.c_str());
        }
        row += 1;        
        if(row == W1.rows()){
            break;
        }
    }  
    getline(loadFile, line);
    if(line != ""){
        std::cout << "Error no seperator found" << std::endl;
       
        loadFile.close();
        return;
    }    
        
    row = 0;
    while (std::getline(loadFile, line))
    {   
        std::stringstream iss(line); 
        for(int col = 0; col < W2.cols(); col++){
            std::string val;
            iss >> val;
            W2(row,col) = std::stof(val.c_str());
        }
        row += 1;        
        if(row == W2.rows()){
            break;
        }
    }
    
    loadFile.close();
 
}




