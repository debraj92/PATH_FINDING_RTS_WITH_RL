//
// Created by Debraj Ray on 2022-06-08.
//

#ifndef EXAMPLE_DQN_INTERFACE_H
#define EXAMPLE_DQN_INTERFACE_H

#include "../observation.h"
#include <atomic>

class DQN_interface {

public:

    atomic<bool> stopLearning;
    bool isTrainingMode;

    DQN_interface(){}
    virtual ~DQN_interface(){}

    virtual void setTrainingMode(bool value) = 0;

    virtual int selectAction(const observation& currentState, int episodeCount, bool *explore) = 0;

    /// Returns the network loss
    virtual double learnWithDQN() = 0;

    virtual void memorizeExperienceForReplay(observation &current, observation &next, int action, float reward, bool done, bool isExploring) = 0;

    /// provide file path
    virtual void saveModel(const string &file) = 0;

    /// provide file path
    virtual void loadModel(const string &file) = 0;

    virtual void updateTargetNet() = 0;

    virtual void decayEpsilon(int currentEpisode) = 0;

    virtual void printAction(int action) = 0;

    virtual void plotLosses() = 0;

    virtual double getBestActionQ() = 0;

};


#endif //EXAMPLE_DQN_INTERFACE_H
