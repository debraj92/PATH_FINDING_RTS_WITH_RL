//
// Created by Debraj Ray on 2022-05-30.
//

#ifndef EXAMPLE_TESTS_RLNN_AGENT_H
#define EXAMPLE_TESTS_RLNN_AGENT_H

#include "DQNNet.h"
#include "../../gameConstants.h"
#include "../DQN_interface.h"
#include "../ReplayMemory.h"
#include <string>

using namespace std;

class RLNN_Agent : public DQN_interface {

    // Hyperparameters
    const double lr = 1e-3;
    // discount factor
    const double gamma = 0.9;
    double epsilon = 1;
    const double epsilon_min = 0.001;
    const double epsilon_decay = 0.99;
    const double alpha = 0.5;
    const int epsilon_annealing_percent = 50;//60;

    int batchSize = 2000;

    unique_ptr<DQNNet> policyNet;
    unique_ptr<DQNNet> targetNet;

    ReplayMemory memory;
    bool isTrainingMode;
    bool startEpsilonDecay;

    bool isExplore(int episodeCount);

public:

    RLNN_Agent() {
        int sizeHiddenLayers = (2 * (MAX_ABSTRACT_OBSERVATIONS + ACTION_SPACE)) / 3;
        policyNet = std::make_unique<DQNNet>(MAX_ABSTRACT_OBSERVATIONS, ACTION_SPACE, sizeHiddenLayers, sizeHiddenLayers, lr, "policyNet");
        targetNet = std::make_unique<DQNNet>(MAX_ABSTRACT_OBSERVATIONS, ACTION_SPACE, sizeHiddenLayers, sizeHiddenLayers, lr, "targetNet");
        // since no learning is performed on the target net
        targetNet->eval();
        if (!isTrainingMode) {
            policyNet->eval();
        }
        startEpsilonDecay = false;
    }

    void setTrainingMode(bool value);

    int selectAction(observation& currentState, int episodeCount, bool *explore);

    double learnWithDQN();

    void memorizeExperienceForReplay(observation &current, observation &next, int action, int reward, bool done);

    /// provide file path
    void saveModel(string &file);

    /// provide file path
    void loadModel(string &file);

    void updateTargetNet();

    void decayEpsilon();

    void printAction(int action);

    void plotLosses();
};


#endif //EXAMPLE_TESTS_RLNN_AGENT_H
