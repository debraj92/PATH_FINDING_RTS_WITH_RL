//
// Created by Debraj Ray on 2022-05-30.
//

#include "DQNNet.h"

#include <utility>
#include <iostream>
#include "../../plot/pbPlots.hpp"
#include "../../plot/supportLib.hpp"

using namespace std;

DQNNet::DQNNet(double learning_rate, const std::string& module_name) :
        m_conv1(nn::Conv2d(torch::nn::Conv2dOptions(MAX_CHANNELS_CNN, 16, /*kernel_size=*/3)))

{
    logger = std::make_unique<Logger>(LogLevel);

    logger->logDebug("Creating DQNNet ")->logDebug(module_name)->endLineDebug();

    auto device = getDeviceType() == "CUDA" ? torch::kCUDA : torch::kCPU;

    m_sequential = nn::Sequential(nn::Linear(INPUT_SIZE, HIDDEN_LAYER_1_SIZE),
                                  nn::ReLU(),
                                  nn::Linear(HIDDEN_LAYER_1_SIZE, HIDDEN_LAYER_2_SIZE),
                                  nn::ReLU(),
                                  nn::Linear(HIDDEN_LAYER_2_SIZE, ACTION_SPACE));

    m_sequential->to(device);
    m_conv1->to(device);
    register_module(module_name, m_sequential);
    register_module(module_name + "_primary_cnn_1", m_conv1);

    optimizer = std::make_unique<optim::Adam>(this->parameters(), torch::optim::AdamOptions(learning_rate));
}


Tensor DQNNet::forwardPass(const Tensor& fov_cnn, const Tensor& inputs_abstraction) {
    logger->logDebug("DQNNet::forwardPass")->endLineDebug();
    optimizer->zero_grad();
    auto cnn_out1 = torch::relu(m_conv1(fov_cnn));
    auto cnn_out = nn::Flatten()(cnn_out1);
    auto cnn_with_abstractions = torch::cat({cnn_out, inputs_abstraction}, 1);
    //cout<<cnn_with_abstractions<<endl;
    return m_sequential->forward(cnn_with_abstractions);
}

void DQNNet::saveModel(const string &file) {
    logger->logInfo("DQNNet::saveModel to file")->endLineInfo();
    torch::save(m_sequential, file + "/vanilla-DQN/model/m_sequential.pt");
    torch::save(m_conv1, file + "/vanilla-DQN/model/m_conv1.pt");
}

void DQNNet::loadModel(const string &file) {
    logger->logDebug("DQNNet::loadModel from file")->endLineDebug();
    torch::load(m_sequential, file + "/vanilla-DQN/model/m_sequential.pt");
    torch::load(m_conv1, file + "/vanilla-DQN/model/m_conv1.pt");
}


double DQNNet::computeLossAndBackPropagate(const Tensor& expected, const Tensor& predicted) {
    logger->logDebug("computeLossAndBackPropagate")->endLineDebug();

    auto lossfn = nn::SmoothL1Loss();

    auto loss = lossfn(predicted, expected);
    double loss_value = loss.data().detach().item<double>();
    logger->logDebug("Network Loss: ")->logDebug(loss_value)->endLineDebug();

    losses.push_back(loss_value);
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

    vector<double> losses_averaged;
    vector<double> episodes;
    int avg_window_size = max(static_cast<int>(losses.size() / MAX_REWARD_POINTS_IN_PLOT), 1);
    for(int i=0; i<losses.size(); i+=avg_window_size) {
        double sum = 0;
        for(int j=i; j<i+avg_window_size and j<losses.size(); j++) {
            sum += losses[j];
        }
        losses_averaged.push_back(sum/avg_window_size);
        episodes.push_back(i+1);
    }

    RGBABitmapImageReference *imageReference = CreateRGBABitmapImageReference();
    StringReference *errorMessage = new StringReference();
    auto success = DrawScatterPlot(imageReference, 1000, 1000, &episodes, &losses_averaged, errorMessage);
    if(success){
        vector<double> *pngdata = ConvertToPNG(imageReference->image);
        WriteToFile(pngdata, "/Users/debrajray/MyComputer/RL-A-STAR-THESIS/plot2/loss_output.png");
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
        case CNN1:
            torch::load(m_conv1, stream);
            break;
    }

}

void DQNNet::saveModel(stringstream &stream, const DQNNet::MODEL_TYPE &model_type) {
    switch(model_type) {
        case SEQUENTIAL:
            torch::save(m_sequential, stream);
            break;
        case CNN1:
            torch::save(m_conv1, stream);
            break;
    }
}


