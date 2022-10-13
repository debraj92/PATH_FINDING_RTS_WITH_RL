//
// Created by Debraj Ray on 2022-05-30.
//

#include "RLNN_Agent.h"

#include <torch/torch.h>
#include <utility>
#include <vector>

#include <iostream>

using namespace std;
using namespace torch;

int RLNN_Agent::selectAction(observation &currentState, int episodeCount, bool *explore) {
    int action;
    *explore = isTrainingMode and isExplore(episodeCount);
    if (*explore) {
        // random action
        cout<<"Selecting random action"<<endl;
        std::uniform_int_distribution<> distri(0, ACTION_SPACE-1);
        std::default_random_engine re;
        re.seed(std::chrono::system_clock::now().time_since_epoch().count());
        action = distri(re);
    } else {

        cout<<"Selecting max action"<<endl;
        float observation_vector[MAX_ABSTRACT_OBSERVATIONS] = {0};
        currentState.flattenObservationToVector(observation_vector);
        auto options = torch::TensorOptions().dtype(torch::kFloat32).device(torch::kCPU);
        Tensor stateTensor = torch::from_blob(observation_vector, {MAX_ABSTRACT_OBSERVATIONS}, options);
        {
            torch::NoGradGuard no_grad;
            auto actions = policyNet->forwardPass(stateTensor.unsqueeze(0));
            action = torch::argmax(actions).detach().item<int>();
            cout<<"Q values at ("<<currentState.playerX<<","<<currentState.playerY<<") : "<<actions<<endl;
        }
    }
    printAction(action);
    return action;
}

double RLNN_Agent::learnWithDQN() {
    cout<<"RLNN_Agent::learn"<<endl;
    // start learning after the replay buffer is partially filled
    if (memory.getBufferSize() < MIN_BUFFERED_EXPERIENCE_FOR_LEARNING) {
        cout<<"Ignoring learning attempt due to insufficient samples";
        return 0;
    }
    if (stopLearning) {
        cout<<"Ignoring learning attempt due to finished training";
        return 0;
    }
    // select n samples picked uniformly at random from the experience replay memory, such that n=batchsize
    memory.sampleBatch(batchSize);

    // states have dimension: batch_size X observation_feature_size
    // output would have dimensions: batch_size X action_space
    auto q_pred = policyNet->forwardPass(memory.tensor_states).gather(1, memory.tensor_actions.view({-1, 1}));

    // targetNet is used for prediction of next state Q value to reduce instability of bootstrapping.
    //auto q_target = get<0>(targetNet->forwardPass1(memory.tensor_next_states).max(1, true));
    auto q_next_state_target = targetNet->forwardPass(memory.tensor_next_states);
    auto q_next_state_policy = policyNet->forwardPass(memory.tensor_next_states);

    auto maximum_from_policy = get<1>(q_next_state_policy.max(1, true));
    auto q_target = q_next_state_target.gather(1, maximum_from_policy);

    // for all terminal states in the batch, set target state q value to 0
    q_target = alpha * gamma * q_target * (1 - memory.tensor_dones.view({-1, 1}));

    auto discounted_reward = alpha * memory.tensor_rewards.view({-1, 1}) + (1 - alpha) * q_pred;

    // calculate expected q value
    auto y = discounted_reward + q_target;

    // calculate the loss as the mean-squared error of y and qpred
    return policyNet->computeLossAndBackPropagate(y, q_pred);
}


void RLNN_Agent::loadModel(string &file) {
    policyNet->loadModel(file);
}

void RLNN_Agent::saveModel(string &file) {
    policyNet->saveModel(file);
}

void RLNN_Agent::updateTargetNet() {
    std::stringstream stream;

    policyNet->saveModel(stream);
    targetNet->loadModel(stream);
}

void RLNN_Agent::decayEpsilon() {
    if (not startEpsilonDecay) {
        return;
    }
    epsilon = max(epsilon_min, epsilon * epsilon_decay);
}

void RLNN_Agent::memorizeExperienceForReplay(observation &current, observation &next, int action, int reward, bool done) {
    memory.storeExperience(current, next, action, reward, done);
}

void RLNN_Agent::printAction(int action) {
    cout<<"RLNN_Agent::printAction"<<endl;
    switch(action) {
        case ACTION_DODGE_LEFT:
            cout<<"ACTION_DODGE_LEFT"<<endl;
            break;
        case ACTION_DODGE_RIGHT:
            cout<<"ACTION_DODGE_RIGHT"<<endl;
            break;
        case ACTION_DODGE_DIAGONAL_LEFT:
            cout<<"ACTION_DODGE_DIAGONAL_LEFT"<<endl;
            break;
        case ACTION_DODGE_DIAGONAL_RIGHT:
            cout<<"ACTION_DODGE_DIAGONAL_RIGHT"<<endl;
            break;
        case ACTION_STRAIGHT:
            cout<<"ACTION_STRAIGHT"<<endl;
            break;
        case ACTION_REROUTE:
            cout<<"ACTION_REROUTE"<<endl;
            break;
        case ACTION_REDIRECT:
            cout<<"ACTION_REDIRECT"<<endl;
            break;
        case ACTION_SWITCH:
            cout<<"ACTION_SWITCH"<<endl;
            break;
        default:
            cout<<"INVALID ACTION "<<action<<endl;
    }
}

void RLNN_Agent::plotLosses() {
    policyNet->plotLoss();
}

bool RLNN_Agent::isExplore(int episodeCount) {

    double episodeCompletion = static_cast<double> (episodeCount) / MAX_EPISODES * 100;
    if(episodeCompletion < epsilon_annealing_percent) {
        return true;
    }
    if (episodeCompletion > SWITCH_TO_EXPLOITATION_ONLY_PERCENT) {
        stopLearning = true;
        return false;
    }
    startEpsilonDecay = true;
    double lower_bound = 0;
    double upper_bound = 1;
    std::uniform_real_distribution<double> unif(lower_bound,upper_bound);
    std::default_random_engine re;
    re.seed(std::chrono::system_clock::now().time_since_epoch().count());
    return unif(re) < epsilon;
}

void RLNN_Agent::setTrainingMode(bool value) {
    isTrainingMode = value;
}
