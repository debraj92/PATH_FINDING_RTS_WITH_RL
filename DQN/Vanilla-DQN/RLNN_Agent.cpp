//
// Created by Debraj Ray on 2022-05-30.
//

#include "RLNN_Agent.h"

#include <torch/torch.h>
#include <utility>
#include <vector>
#include <testing.h>
#include <iostream>

using namespace std;
using namespace torch;

int RLNN_Agent::selectAction(const observation &currentState, int episodeCount, bool *explore) {

    int action;
    *explore = isTrainingMode and isExplore(episodeCount);
    if (*explore) {
        // random action
        logger->logDebug("Selecting random action")->endLineDebug();
        std::uniform_int_distribution<> distri(0, ACTION_SPACE-1);
        std::default_random_engine re;
#ifdef TESTING
        re.seed(seedAction++);
#else
        re.seed(std::chrono::system_clock::now().time_since_epoch().count());
#endif
        action = distri(re);
    } else {
        logger->logDebug("Selecting max action")->endLineDebug();
        auto options = torch::TensorOptions().dtype(torch::kFloat32);

        float obstaclesFOV[1][FOV_WIDTH][FOV_WIDTH];
        float enemiesFOV[1][FOV_WIDTH][FOV_WIDTH];
        float pathFOV[1][FOV_WIDTH][FOV_WIDTH];
        copy(&currentState.obstaclesFOV[0][0], &currentState.obstaclesFOV[0][0] + FOV_WIDTH * FOV_WIDTH, &obstaclesFOV[0][0][0]);
        copy(&currentState.enemiesFOV[0][0], &currentState.enemiesFOV[0][0] + FOV_WIDTH * FOV_WIDTH, &enemiesFOV[0][0][0]);
        copy(&currentState.pathFOV[0][0], &currentState.pathFOV[0][0] + FOV_WIDTH * FOV_WIDTH, &pathFOV[0][0][0]);
        auto tensor_obstacles = torch::from_blob(obstaclesFOV, {1, FOV_WIDTH, FOV_WIDTH}, options).to(device).unsqueeze(1);
        auto tensor_enemies = torch::from_blob(enemiesFOV, {1, FOV_WIDTH, FOV_WIDTH}, options).to(device).unsqueeze(1);
        auto tensor_path = torch::from_blob(pathFOV, {1, FOV_WIDTH, FOV_WIDTH}, options).to(device).unsqueeze(1);
        auto tensor_fov_channels= torch::cat({tensor_obstacles, tensor_enemies, tensor_path}, 1).to(device);

        float observation_vector[MAX_ABSTRACT_OBSERVATIONS] = {0};
        currentState.flattenObservationToVector(observation_vector);
        auto tensor_states = torch::zeros({1, MAX_ABSTRACT_OBSERVATIONS}, options).to(device);
        tensor_states.slice(0, 0, 1) = torch::from_blob(observation_vector, {MAX_ABSTRACT_OBSERVATIONS}, options).to(device);
        Tensor actions;
        {
            // critical
            std::lock_guard<mutex> locker(safeActionSelectionAndTraining);
            actions = policyNetSaved->forwardPass(tensor_fov_channels, tensor_states);
        }
        action = torch::argmax(actions).detach().item<int>();
        bestActionQValue = actions[0][action].detach().item<double>();
        /*
        cout<<"Q values at ("<<currentState.playerX<<","<<currentState.playerY<<") : "<<actions<<endl;
        cout<<"Best Q "<<bestActionQValue<<endl;
        cout<<"direction: "<<currentState.direction<<endl;
        cout<<endl;
        cout<<"Abstract State:\n"<<tensor_states<<endl;
        cout<<"FOV\n"<<tensor_fov_channels<<endl;
        */
    }

    printAction(action);
    return action;
}

double RLNN_Agent::learnWithDQN() {
    logger->logDebug("RLNN_Agent::learn")->endLineDebug();
    // start learning after the replay buffer is partially filled
    if (memory.getBufferSize() < MIN_BUFFERED_EXPERIENCE_FOR_LEARNING) {
        logger->logDebug("Ignoring learning attempt due to insufficient samples")->endLineDebug();
        return 0;
    }
    if (stopLearning) {
        logger->logDebug("Ignoring learning attempt due to finished training")->endLineDebug();
        return 0;
    }
    // select n samples picked uniformly at random from the experience replay memory, such that n=batchsize
    memory.sampleBatch(batchSize);

    // states have dimension: batch_size X observation_feature_size
    // output would have dimensions: batch_size X action_space

    auto q_pred =  policyNet->forwardPass(memory.tensor_fov_channels_current, memory.tensor_states)
            .gather(1, memory.tensor_actions.view({-1, 1}));

    // targetNet is used for prediction of next state Q value to reduce instability of bootstrapping.
    auto q_target = get<0>(targetNet->forwardPass(memory.tensor_fov_channels_next, memory.tensor_next_states).max(1, true));

    // for all terminal states in the batch, set target state q value to 0
    q_target = alpha * gamma * q_target * (1 - memory.tensor_dones.view({-1, 1}));

    auto discounted_reward = alpha * memory.tensor_rewards.view({-1, 1}) + (1 - alpha) * q_pred;

    // calculate expected q value
    auto y = discounted_reward + q_target;

    // calculate the loss as the mean-squared error of y and qpred
    auto loss = policyNet->computeLossAndBackPropagate(y, q_pred);

    /// Save policy net for action selection
    savePolicyNet();

    return loss;
}


void RLNN_Agent::loadModel(const string &file) {
    policyNet->loadModel(file);
    policyNetSaved->loadModel(file);
    targetNet->loadModel(file);
}

void RLNN_Agent::saveModel(const string &file) {
    policyNet->saveModel(file);
}

void RLNN_Agent::updateTargetNet() {
    std::stringstream stream1, stream2, stream3;

    policyNet->saveModel(stream1, DQNNet::SEQUENTIAL);
    targetNet->loadModel(stream1, DQNNet::SEQUENTIAL);
    policyNet->saveModel(stream2, DQNNet::CNN1);
    targetNet->loadModel(stream2, DQNNet::CNN1);
}

void RLNN_Agent::decayEpsilon(int currentEpisode) {
    if (not startEpsilonDecay) {
        return;
    }
    if (currentEpisode % ((MAX_EPISODES * (100 - EXPLOITATION_START_PERCENT)) / (100 * 1150)) == 0) {
        epsilon = max(epsilon_min, epsilon * epsilon_decay);
    }
}

void RLNN_Agent::memorizeExperienceForReplay(observation &current, observation &next, int action, float reward, bool done, bool isExploring) {
    memory.storeExperience(current, next, action, reward, done, isExploring);
}

void RLNN_Agent::printAction(int action) {
    logger->logDebug("RLNN_Agent::printAction ");
    switch(action) {
        case ACTION_DODGE_DIAGONAL_LEFT:
            logger->logDebug("ACTION_DODGE_DIAGONAL_LEFT")->endLineDebug();
            break;
        case ACTION_DODGE_DIAGONAL_RIGHT:
            logger->logDebug("ACTION_DODGE_DIAGONAL_RIGHT")->endLineDebug();
            break;
        case ACTION_STRAIGHT:
            logger->logDebug("ACTION_STRAIGHT")->endLineDebug();
            break;
        case ACTION_DODGE_LEFT:
            logger->logDebug("ACTION_DODGE_LEFT")->endLineDebug();
            break;
        case ACTION_DODGE_RIGHT:
            logger->logDebug("ACTION_DODGE_RIGHT")->endLineDebug();
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
#ifdef TESTING
    re.seed(seedExplore++);
#else
    re.seed(std::chrono::system_clock::now().time_since_epoch().count());
#endif

    return unif(re) < epsilon;
}

void RLNN_Agent::setTrainingMode(bool value) {
    isTrainingMode = value;
    if (!isTrainingMode) {
        policyNet->eval();
    }
}

void RLNN_Agent::savePolicyNet() {
    std::stringstream stream1, stream2, stream3;
    policyNet->saveModel(stream1, DQNNet::SEQUENTIAL);
    policyNet->saveModel(stream2, DQNNet::CNN1);
    {
        // critical
        std::lock_guard<mutex> locker(safeActionSelectionAndTraining);
        policyNetSaved->loadModel(stream1, DQNNet::SEQUENTIAL);
        policyNetSaved->loadModel(stream2, DQNNet::CNN1);
    }
}

double RLNN_Agent::getBestActionQ() {
    return bestActionQValue;
}
