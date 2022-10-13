//
// Created by Debraj Ray on 2022-05-30.
//

#ifndef EXAMPLE_TESTS_DQNNET_H
#define EXAMPLE_TESTS_DQNNET_H

#include <torch/torch.h>
#include <vector>
#include <string>
#include "../../gameConstants.h"
#include "../../Logger.h"

using namespace torch;
using namespace std;

class DQNNet : public nn::Module {

    const LOG_LEVEL LogLevel = LOG_LEVEL::INFO;
    std::unique_ptr<Logger> logger;

    nn::Sequential m_sequential;
    torch::nn::Conv2d m_conv1;

    int count = 1;

    unique_ptr<optim::Adam> optimizer;

    vector<double> losses;

    string getDeviceType()
    {
        char * val = getenv("DEVICE_TYPE");
        return val == NULL ? "CPU" : std::string(val);
    }

public:

    enum MODEL_TYPE{
        SEQUENTIAL,
        CNN1
    };

    DQNNet(double learning_rate, const std::string& module_name);

    Tensor forwardPass(const Tensor& fov_cnn, const Tensor& inputs_abstraction);

    void saveModel(const string &file);

    void loadModel(const string &file);

    void saveModel(stringstream &stream, const DQNNet::MODEL_TYPE &model_type);

    void loadModel(stringstream &stream, const DQNNet::MODEL_TYPE &model_type);

    double computeLossAndBackPropagate(const Tensor& expected, const Tensor& predicted);

    void plotLoss();
};


#endif //EXAMPLE_TESTS_DQNNET_H
