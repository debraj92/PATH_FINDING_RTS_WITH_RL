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
#include "../../plot/pbPlots.hpp"
#include "../../plot/supportLib.hpp"

using namespace torch;
using namespace std;

class DQNNet : public nn::Module {

    const LOG_LEVEL LogLevel = LOG_LEVEL::INFO;
    std::unique_ptr<Logger> logger;

    nn::Sequential m_sequential;

    nn::Linear m_value;

    nn::Linear m_advantage;

    torch::nn::Conv2d m_conv1;

    int count = 1;

    unique_ptr<optim::Adam> optimizer;

    vector<double> losses;

public:

    enum MODEL_TYPE{
        SEQUENTIAL,
        VALUE,
        ADVANTAGE,
        CNN1
    };

    DQNNet(double learning_rate, const std::string& module_name);

    Tensor forwardPass(const Tensor& fov_cnn, const Tensor& inputs_abstraction);

    Tensor forwardPassValue(const Tensor& inputs);

    Tensor forwardPassAdvantage(const Tensor& inputs);

    void saveModel(const string &file);

    void loadModel(const string &file);

    void saveModel(stringstream &stream, const MODEL_TYPE &model_type);

    void loadModel(stringstream &stream, const MODEL_TYPE &model_type);

    double computeLossAndBackPropagate(const Tensor& expected, const Tensor& predicted);

    void plotLoss();
};


#endif //EXAMPLE_TESTS_DQNNET_H
