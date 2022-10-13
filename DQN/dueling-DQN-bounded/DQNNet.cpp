//
// Created by Debraj Ray on 2022-05-30.
//

#include "DQNNet.h"

#include <utility>
#include <iostream>
#include "../../plot/pbPlots.hpp"
#include "../../plot/supportLib.hpp"
#include "../../gameConstants.h"

using namespace std;

DQNNet::DQNNet(int inputSize, int outputSize, int hiddenLayer1Size, int hiddenLayer2Size, double learning_rate, const std::string& module_name) : m_value(hiddenLayer1Size, 1),
                                                                                                                                                  m_advantage(hiddenLayer1Size, outputSize)
{
    logger = std::make_unique<Logger>(LogLevel);

    logger->logDebug("Creating DQNNet ")->logDebug(module_name)->endLineDebug();

    m_sequential = nn::Sequential(nn::Linear(inputSize, hiddenLayer1Size),
                                  nn::Sigmoid());

    auto celu_options = nn::CELUOptions();
    celu_options.alpha(REWARD_REACH * 0.2);
    m_lower_bound = nn::CELU(celu_options);

    register_module(module_name + "_primary", m_sequential);
    register_module(module_name + "_secondary_value", m_value);
    register_module(module_name + "_secondary_advantage", m_advantage);
    register_module(module_name + "_secondary_lower_bound", m_lower_bound);

    optimizer = std::make_unique<optim::Adam>(this->parameters(), torch::optim::AdamOptions(learning_rate));
}

Tensor DQNNet::forwardPass1(const Tensor& inputs) {
    logger->logDebug("DQNNet::forwardPass1")->endLineDebug();
    optimizer->zero_grad();
    return m_sequential->forward(inputs);
}

Tensor DQNNet::forwardPassValue(const Tensor &inputs) {
    return m_value(inputs);
}

Tensor DQNNet::forwardPassAdvantage(const Tensor &inputs) {
    return m_advantage(inputs);
}

/// For lower bound - CELU
Tensor DQNNet::forwardPass2(const Tensor& input1, const Tensor& input2) {
    logger->logDebug("DQNNet::forwardPass2")->endLineDebug();
    return m_lower_bound(input1 + input2);
}


void DQNNet::saveModel(string &file) {
    logger->logDebug("DQNNet::saveModel from file")->endLineDebug();
    torch::save(m_sequential, file + "/m_sequential.pt");
    torch::save(m_value, file + "/m_value.pt");
    torch::save(m_advantage, file + "/m_adv.pt");
    torch::save(m_lower_bound, file + "/m_lower_bound.pt");
}

void DQNNet::loadModel(string &file) {
    logger->logDebug("DQNNet::loadModel from file")->endLineDebug();
    torch::load(m_sequential, file + "/m_sequential.pt");
    torch::load(m_value, file + "/m_value.pt");
    torch::load(m_advantage, file + "/m_adv.pt");
    torch::load(m_lower_bound, file + "/m_lower_bound.pt");
}


double DQNNet::computeLossAndBackPropagate(const Tensor& expected, const Tensor& predicted) {
    logger->logDebug("computeLossAndBackPropagate")->endLineDebug();
    auto lossfn = nn::SmoothL1Loss();

    auto loss = lossfn(predicted, expected);
    double loss_value = loss.data().detach().item<double>();
    logger->logDebug("Network Loss: ")->logDebug(loss_value)->endLineDebug();
    losses.push_back(loss_value);
    loss_count.push_back(count++);

    loss.backward();

    /// Gradient clipping
    for (auto &param: this->parameters()) {
        param.grad().data().clamp_(-1, 1);
    }

    optimizer->step();

    return loss_value;

}


void DQNNet::plotLoss() {
    logger->logDebug("Network Loss")->endLineDebug();

    RGBABitmapImageReference *imageReference = CreateRGBABitmapImageReference();
    StringReference *errorMessage = new StringReference();
    auto success = DrawScatterPlot(imageReference, 1000, 1000, &loss_count, &losses, errorMessage);
    if(success){
        vector<double> *pngdata = ConvertToPNG(imageReference->image);
        WriteToFile(pngdata, "/Users/debrajray/MyComputer/RL-A-STAR-THESIS/plot/loss_output.png");
        DeleteImage(imageReference->image);
    }else{
        cerr << "Error: ";
        for(wchar_t c : *errorMessage->string){
            wcerr << c;
        }
        cerr << endl;
    }

}

void DQNNet::loadModel(stringstream &stream, const DQNNet::MODEL_TYPE &model_type) {
    switch(model_type) {
        case SEQUENTIAL:
            torch::load(m_sequential, stream);
            break;
        case VALUE:
            torch::load(m_value, stream);
            break;
        case ADVANTAGE:
            torch::load(m_advantage, stream);
            break;
        case LOWER_BOUND:
            torch::load(m_lower_bound, stream);
            break;
    }
}

void DQNNet::saveModel(stringstream &stream, const DQNNet::MODEL_TYPE &model_type) {
    switch(model_type) {
        case SEQUENTIAL:
            torch::save(m_sequential, stream);
            break;
        case VALUE:
            torch::save(m_value, stream);
            break;
        case ADVANTAGE:
            torch::save(m_advantage, stream);
            break;
        case LOWER_BOUND:
            torch::save(m_lower_bound, stream);
            break;
    }
}


