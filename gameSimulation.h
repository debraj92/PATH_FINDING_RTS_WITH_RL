//
// Created by Debraj Ray on 2021-12-30.
//

#ifndef EXAMPLE_GAMESIMULATION_H
#define EXAMPLE_GAMESIMULATION_H

#include <iostream>
#include "vector"
#include "enemy/enemy.h"
#include "gameConstants.h"
#include "player.h"
#include "findPath.h"
#include "coordinatesUtil.h"
#include "enemy/enemyUIData.h"


using namespace std;

class gameSimulation : public coordinatesUtil {

    const LOG_LEVEL LogLevel = LOG_LEVEL::INFO;

    std::unique_ptr<Logger> logger;

    unordered_set<int> enemiesAwayFromBase;

    int movePlayer(vector<vector<int>> &grid, const observation &currentObservation, int* error);
    void moveEnemies(vector<std::vector<int>> &grid, observation &ob, int time);
    void fight(vector<std::vector<int>> &grid);
    float calculateReward(const observation &nextObservation, int action, int action_error);
    void populateEnemies(std::vector<std::vector<int>> &grid, bool isTrainingMode);

    bool isEpisodeComplete();

    void headStraightToDestination(vector<vector<int>> &grid, vector<enemyUIData> &enemiesInThisRound);

    bool isMDPDone(observation &nextObservation);

    void populateEnemiesForUI(vector<enemyUIData> &enemiesUI);

    void markDeadEnemies(vector<enemyUIData> &enemiesUI);

    bool isStuckAtBorder();

public:

    explicit gameSimulation(vector<vector<int>> &grid) : coordinatesUtil(grid) {
        logger = std::make_unique<Logger>(LogLevel);
    }

    player* player1;

    void learnToPlay(std::vector<std::vector<int>> &grid);

    void play(std::vector<std::vector<int>> &grid);

    float getTotalRewardsCollected();

    void removeCharacters(std::vector<std::vector<int>> &grid);

    bool isDestinationReached();

    /// Testing
#ifdef TESTING

    friend class Simulation_test;
    FRIEND_TEST(Simulation_test, test3);
    FRIEND_TEST(Simulation_test, test4);

#endif

};


#endif //EXAMPLE_GAMESIMULATION_H
