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
        logger->logDebug("Selecting random action")->endLineDebug();
        std::uniform_int_distribution<> distri(0, ACTION_SPACE-1);
        std::default_random_engine re;
        re.seed(std::chrono::system_clock::now().time_since_epoch().count());
        action = distri(re);
    } else {
        logger->logDebug("Selecting max action")->endLineDebug();
        float observation_vector[MAX_ABSTRACT_OBSERVATIONS] = {0};
        currentState.flattenObservationToVector(observation_vector);
        auto options = torch::TensorOptions().dtype(torch::kFloat32).device(torch::kCPU);
        Tensor stateTensor = torch::from_blob(observation_vector, {MAX_ABSTRACT_OBSERVATIONS}, options);
        {
            torch::NoGradGuard no_grad;
            auto fp1 = policyNet->forwardPass1(stateTensor.unsqueeze(0));
            auto actions = policyNet->forwardPassAdvantage(fp1);
            action = torch::argmax(actions).detach().item<int>();
            //cout<<"Q values at ("<<currentState.playerX<<","<<currentState.playerY<<") : "<<actions<<endl;
        }
    }
    printAction(action);
    return action;
}

/// Duelling DQN Bounded
double RLNN_Agent::learnWithDQN() {
    logger->logDebug("RLNN_Agent::learn")->endLineDebug();
    // start learning after the replay buffer is partially filled
    if (memory.getBufferSize() < MIN_BUFFERED_EXPERIENCE_FOR_LEARNING) {
        logger->logDebug("Ignoring learning attempt due to insufficient samples")->endLineDebug();
        // Dummy
        return 0;
    }
    if (stopLearning) {
        logger->logDebug("Ignoring learning attempt due to finished training")->endLineDebug();
        // Dummy
        return 0;
    }
    // select n samples picked uniformly at random from the experience replay memory, such that n=batchsize
    memory.sampleBatch(batchSize);

    // states have dimension: batch_size X observation_feature_size
    // output would have dimensions: batch_size X action_space

    auto fp1 = policyNet->forwardPass1(memory.tensor_states);
    auto s_v = policyNet->forwardPassValue(fp1);
    auto s_a = policyNet->forwardPassAdvantage(fp1);

    auto fp3 = policyNet->forwardPass1(memory.tensor_next_states);
    auto ns_v = policyNet->forwardPassValue(fp3);
    auto ns_a = policyNet->forwardPassAdvantage(fp3);

    auto fp2 = targetNet->forwardPass1(memory.tensor_next_states);
    auto nst_v = targetNet->forwardPassValue(fp2);
    auto nst_a = targetNet->forwardPassAdvantage(fp2);


    auto q_pred = torch::add(s_v, s_a - s_a.mean(1, true)).gather(1, memory.tensor_actions.view({-1, 1}));
    auto q_next = torch::add(nst_v, nst_a - nst_a.mean(1, true));
    auto q_eval = torch::add(ns_v, ns_a - ns_a.mean(1, true));

    auto maximum_from_policy = get<1>(q_eval.max(1, true));
    auto q_target = q_next.gather(1, maximum_from_policy);


    q_pred = policyNet->forwardPass2(q_pred, torch::zeros_like(q_pred));

    q_target = alpha * gamma * q_target * (1 - memory.tensor_dones.view({-1, 1}));

    auto discounted_reward = alpha * memory.tensor_rewards.view({-1, 1}) + (1 - alpha) * q_pred;

    // calculate expected q value
    auto y = targetNet->forwardPass2(discounted_reward, q_target);

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
    std::stringstream stream1, stream2, stream3, stream4;

    policyNet->saveModel(stream1, DQNNet::SEQUENTIAL);
    targetNet->loadModel(stream1, DQNNet::SEQUENTIAL);
    policyNet->saveModel(stream2, DQNNet::VALUE);
    targetNet->loadModel(stream2, DQNNet::VALUE);
    policyNet->saveModel(stream3, DQNNet::ADVANTAGE);
    targetNet->loadModel(stream3, DQNNet::ADVANTAGE);
    policyNet->saveModel(stream4, DQNNet::LOWER_BOUND);
    targetNet->loadModel(stream4, DQNNet::LOWER_BOUND);
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
    logger->logDebug("RLNN_Agent::printAction ");
    switch(action) {
        case ACTION_DODGE_LEFT:
            logger->logDebug("ACTION_DODGE_LEFT")->endLineDebug();
            break;
        case ACTION_DODGE_RIGHT:
            logger->logDebug("ACTION_DODGE_RIGHT")->endLineDebug();
            break;
        case ACTION_DODGE_DIAGONAL_LEFT:
            logger->logDebug("ACTION_DODGE_DIAGONAL_LEFT")->endLineDebug();
            break;
        case ACTION_DODGE_DIAGONAL_RIGHT:
            logger->logDebug("ACTION_DODGE_DIAGONAL_RIGHT")->endLineDebug();
            break;
        case ACTION_STRAIGHT:
            logger->logDebug("ACTION_STRAIGHT")->endLineDebug();
            break;
        case ACTION_REROUTE:
            logger->logDebug("ACTION_REROUTE")->endLineDebug();
            break;
        case ACTION_REDIRECT:
            logger->logDebug("ACTION_REDIRECT")->endLineDebug();
            break;
        case ACTION_SWITCH:
            logger->logDebug("ACTION_SWITCH")->endLineDebug();
            break;
        default:
            logger->logDebug("INVALID ACTION")->endLineDebug();
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
