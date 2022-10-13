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
#include <testing.h>

using namespace std;

class RLNN_Agent : public DQN_interface {

    const LOG_LEVEL LogLevel = LOG_LEVEL::INFO;
    std::unique_ptr<Logger> logger;

    // Hyperparameters
    const double lr = 1e-3;
    // discount factor
    const double gamma = 0.9;
    double epsilon = 1;
    const double epsilon_min = 0.01;
    const double epsilon_decay = 0.998;
    const double alpha = 1;
    const int epsilon_annealing_percent = 60;

    int batchSize = 4000;

    unique_ptr<DQNNet> policyNet;
    unique_ptr<DQNNet> targetNet;

    ReplayMemory memory;
    bool isTrainingMode;
    bool startEpsilonDecay;

    CNN_controller& cnn;

    bool isExplore(int episodeCount);

public:

    RLNN_Agent(CNN_controller& cnn1) : memory(cnn1), cnn(cnn1) {
        policyNet = std::make_unique<DQNNet>(lr, "policyNet");
        targetNet = std::make_unique<DQNNet>(lr, "targetNet");
        // since no learning is performed on the target net
        targetNet->eval();
        startEpsilonDecay = false;
        logger = std::make_unique<Logger>(LogLevel);
    }

    void setTrainingMode(bool value);

    int selectAction(observation& currentState, int episodeCount, bool *explore);

    double learnWithDQN();

    // TODO: Interface changes in other DQNs
    void memorizeExperienceForReplay(observation &current, observation &next, int action, float reward, bool done, bool isExploring);

    /// provide file path
    void saveModel(const string &file);

    /// provide file path
    void loadModel(const string &file);

    void updateTargetNet();

    void decayEpsilon();

    void printAction(int action);

    void plotLosses();

#ifdef TESTING

    /// For testing only
    int seedAction = 1;
    int seedExplore = 2;

    virtual ReplayMemory* getAccessToReplayMemory() {
        return &memory;
    }

#endif
};


#endif //EXAMPLE_TESTS_RLNN_AGENT_H
