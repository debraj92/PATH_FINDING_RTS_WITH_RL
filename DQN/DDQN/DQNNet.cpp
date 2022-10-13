//
// Created by Debraj Ray on 2022-05-30.
//

#include "DQNNet.h"

#include <utility>
#include <iostream>
#include "../../plot/pbPlots.hpp"
#include "../../plot/supportLib.hpp"

using namespace std;

DQNNet::DQNNet(int inputSize, int outputSize, int hiddenLayer1Size, int hiddenLayer2Size, double learning_rate, const std::string& module_name)
{
    cout<<"Creating DQNNet "<<module_name<<endl;

    m_sequential = nn::Sequential(nn::Linear(inputSize, hiddenLayer1Size),
                                  nn::Sigmoid(),
                                  nn::Linear(hiddenLayer1Size, outputSize));

    register_module(module_name, m_sequential);

    optimizer = std::make_unique<optim::Adam>(this->parameters(), torch::optim::AdamOptions(learning_rate));
}

Tensor DQNNet::forwardPass(const Tensor& inputs) {
    cout<<"DQNNet::forwardPass"<<endl;
    optimizer->zero_grad();
    return m_sequential->forward(inputs);
}

void DQNNet::saveModel(string &file) {
    cout<<"DQNNet::saveModel"<<endl;
    torch::save(m_sequential, file + "/m_sequential.pt");
}

void DQNNet::loadModel(string &file) {
    cout<<"DQNNet::loadModel"<<endl;
    torch::load(m_sequential, file + "/m_sequential.pt");
}


double DQNNet::computeLossAndBackPropagate(const Tensor& expected, const Tensor& predicted) {
    cout<<"computeLossAndBackPropagate "<<endl;

    auto lossfn = nn::SmoothL1Loss();

    auto loss = lossfn(predicted, expected);
    cout<<"Network Loss: "<<loss<<endl;
    double loss_value = loss.data().detach().item<double>();
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
    cout<<"Network Loss"<<endl;
    /*for(double loss: losses) {
        cout<<loss<<endl;
    }*/
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

void DQNNet::loadModel(stringstream &stream) {
    torch::load(m_sequential, stream);
}

void DQNNet::saveModel(stringstream &stream) {
    torch::save(m_sequential, stream);
}


