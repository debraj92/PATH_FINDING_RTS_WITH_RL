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
using namespace RTS;

class DQNNet : public nn::Module {

    const LOG_LEVEL LogLevel = LOG_LEVEL::INFO;
    std::unique_ptr<Logger> logger;

    nn::Sequential m_sequential;

    nn::Linear m_value;

    nn::Linear m_advantage;

    nn::CELU m_lower_bound;

    int count = 1;

    unique_ptr<optim::Adam> optimizer;

    vector<double> losses;
    vector<double> loss_count;

public:

    enum MODEL_TYPE{
        SEQUENTIAL,
        VALUE,
        ADVANTAGE,
        LOWER_BOUND
    };

    DQNNet(int inputSize, int outputSize, int hiddenLayer1Size, int hiddenLayer2Size, double learning_rate, const std::string& module_name);

    Tensor forwardPass1(const Tensor& inputs);

    Tensor forwardPassValue(const Tensor& inputs);

    Tensor forwardPassAdvantage(const Tensor& inputs);

    Tensor forwardPass2(const Tensor& input1, const Tensor& input2);

    void saveModel(string &file);

    void loadModel(string &file);

    void saveModel(stringstream &stream, const MODEL_TYPE &model_type);

    void loadModel(stringstream &stream, const MODEL_TYPE &model_type);

    double computeLossAndBackPropagate(const Tensor& expected, const Tensor& predicted);

    void plotLoss();
};


#endif //EXAMPLE_TESTS_DQNNET_H
