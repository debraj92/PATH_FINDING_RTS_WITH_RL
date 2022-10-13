//
// Created by Debraj Ray on 2022-05-28.
//

#ifndef EXAMPLE_TESTS_REPLAYMEMORY_H
#define EXAMPLE_TESTS_REPLAYMEMORY_H

#include <torch/torch.h>
#include <memory>
#include <vector>
#include <iostream>


#include <algorithm>
#include <iterator>
#include <random>
#include <vector>

#include "../observation.h"
#include "../gameConstants.h"

#include "../FOV_CNN/CNN_controller.h"

#include <testing.h>

#include <mutex>

/// Testing
#ifdef TESTING
#include <gtest/gtest.h>
#endif

//#define ENABLE_STATE_VECTOR_DUMP 1

using namespace std;
class ReplayMemory {

    const LOG_LEVEL LogLevel = LOG_LEVEL::INFO;
    std::unique_ptr<Logger> logger;

    std::mutex replayBuffersSafeReadWrite;

    ///// NEEDS TO BE PROTECTED FROM CONCURRENT READ/WRITE /////////////////////////
    vector<vector<float>> buffer_states;
    vector<vector<float>> buffer_next_states;
    vector<long> buffer_actions;
    vector<float> rewards;
    vector<bool> dones;

    /// FOV for CNN - current state
    float (*obstaclesFOVcurrent)[FOV_WIDTH][FOV_WIDTH] = new float [MAX_CAPACITY_REPLAY_BUFFER][FOV_WIDTH][FOV_WIDTH]();
    float (*enemiesFOVcurrent)[FOV_WIDTH][FOV_WIDTH] = new float [MAX_CAPACITY_REPLAY_BUFFER][FOV_WIDTH][FOV_WIDTH]();
    float (*pathFOVcurrent)[FOV_WIDTH][FOV_WIDTH] = new float [MAX_CAPACITY_REPLAY_BUFFER][FOV_WIDTH][FOV_WIDTH]();

    /// FOV for CNN - next state
    float (*obstaclesFOVnext)[FOV_WIDTH][FOV_WIDTH] = new float [MAX_CAPACITY_REPLAY_BUFFER][FOV_WIDTH][FOV_WIDTH]();
    float (*enemiesFOVnext)[FOV_WIDTH][FOV_WIDTH] = new float [MAX_CAPACITY_REPLAY_BUFFER][FOV_WIDTH][FOV_WIDTH]();
    float (*pathFOVnext)[FOV_WIDTH][FOV_WIDTH] = new float [MAX_CAPACITY_REPLAY_BUFFER][FOV_WIDTH][FOV_WIDTH]();

    ///////////////////////////////////////////////////////////////////////////

    int idx = 0;

    bool isBufferFull = false;

    double exploitation_window_start = MIN_EXPLOITATION_WINDOW_START_FOR_MEMORY;

    void storeExperience(observation &current, observation &next, int action, float reward, bool done);

    string getDeviceType()
    {
        char * val = getenv("DEVICE_TYPE");
        return val == NULL ? "CPU" : std::string(val);
    }

public:

    torch::Tensor tensor_states;
    torch::Tensor tensor_next_states;
    torch::Tensor tensor_actions;
    torch::Tensor tensor_rewards;
    torch::Tensor tensor_dones;
    torch::DeviceType device;

    // CNN
    torch::Tensor tensor_fov_channels_current;
    torch::Tensor tensor_fov_channels_next;

    ReplayMemory() : buffer_states(MAX_CAPACITY_REPLAY_BUFFER, vector<float>(MAX_ABSTRACT_OBSERVATIONS, 0)),
                     buffer_next_states(MAX_CAPACITY_REPLAY_BUFFER, vector<float>(MAX_ABSTRACT_OBSERVATIONS, 0)),
                     buffer_actions(MAX_CAPACITY_REPLAY_BUFFER),
                     rewards(MAX_CAPACITY_REPLAY_BUFFER),
                     dones(MAX_CAPACITY_REPLAY_BUFFER)
    {
        logger = std::make_unique<Logger>(LogLevel);

        // initialize
        for (int i=0; i<MAX_CAPACITY_REPLAY_BUFFER; i++) {
            for (int j=0; j<FOV_WIDTH; j++) {
                for (int k=0; k<FOV_WIDTH; k++) {
                    obstaclesFOVcurrent[i][j][k] = 0;
                    enemiesFOVcurrent[i][j][k] = 0;
                    pathFOVcurrent[i][j][k] = 0;

                    obstaclesFOVnext[i][j][k] = 0;
                    enemiesFOVnext[i][j][k] = 0;
                    pathFOVnext[i][j][k] = 0;
                }
            }
        }

        device = getDeviceType() == "CUDA" ? torch::kCUDA : torch::kCPU;

#ifdef ENABLE_STATE_VECTOR_DUMP
        logger->openLogFile();
#endif

    }

    ~ReplayMemory() {
#ifdef ENABLE_STATE_VECTOR_DUMP
        logger->closeLogFile();
#endif
        delete[] obstaclesFOVcurrent;
        delete[] enemiesFOVcurrent;
        delete[] pathFOVcurrent;

        delete[] obstaclesFOVnext;
        delete[] enemiesFOVnext;
        delete[] pathFOVnext;
    }

    void sampleBatch(int batchSize);

    void storeExperience(observation &current, observation &next, int action, float reward, bool done, bool isExploring);

    int getBufferSize();

    void logStateVector(observation &ob);

    /// Testing
#ifdef TESTING

    int seedSamplingBatch = 1;

    friend class ReplayMemory_test;
    FRIEND_TEST(ReplayMemory_test, test_cnn);

    friend class Simulation_test;
    FRIEND_TEST(Simulation_test, test1);
    FRIEND_TEST(Simulation_test, test2);
    FRIEND_TEST(Simulation_test, test3);

#endif

};


#endif //EXAMPLE_TESTS_REPLAYMEMORY_H
