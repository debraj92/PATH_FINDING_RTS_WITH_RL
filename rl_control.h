//
// Created by Debraj Ray on 2022-01-03.
//

#ifndef EXAMPLE_RL_CONTROL_H
#define EXAMPLE_RL_CONTROL_H

#include "state.h"
#include "observation.h"
#include "vector"
//#include "gameSimulation.h"

using namespace std;

class rl_control {

    state states[MAX_STATES];

    int nextFreeSlot = 0;

    // Hyperparameters
    double alpha = 0.1;
    double gamma = 0.6;
    double epsilon = 0.3;
    double epsilon_decay = 0.9;

    // Q(st, at)
    double getCurrentQValue(state* current_state, int current_action);

    // max  Q(st+1,a)
    //    a
    double getMaxQ(state* given_state);

    int getStateFromObservation(observation& ob);

    void createNonTerminalState(observation& ob);

    bool checkObstacles(observation& ob, state &s);

    bool compareEnemies(observation& ob, state &s);

public:

    state *cur_state;

    state* evaluateActionProbabilities(int reward, observation& next_observation, int current_action);

    state* getNextStateFromObservation(observation& next_observation);

    int getNextAction();

    int getNextActionForInference();

    void createStartState(observation& ob);

    void make_greedy();

    void printAllStatesAndPolicies();

    void copyObstacleInfo(observation& ob, state& new_state);

    void createModel();

    void restoreModel();

};


#endif //EXAMPLE_RL_CONTROL_H
