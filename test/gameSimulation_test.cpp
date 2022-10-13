//
// Created by Debraj Ray on 2022-06-21.
//

#include <gtest/gtest.h>
#include "../gameConstants.h"
#include "../fixedobstacles.h"
#include "../gameSimulation.h"
#include "../player.h"
#include "../Logger.h"
#include "../enemy/enemy.h"

using namespace std;

class Simulation_test : public ::testing::Test {

};

void createAllFixedObstacles(vector<std::vector<int>> &grid, int TOTAL_FIXED_OBSTACLES, int blockObstacles[][4]) {

    // fill with static obstacles
    FixedObstacles fixedObstacles;
    for(int obstacle=0; obstacle<TOTAL_FIXED_OBSTACLES; obstacle++) {
        int x_s = blockObstacles[obstacle][0];
        int x_e = blockObstacles[obstacle][1];
        int y_s = blockObstacles[obstacle][2];
        int y_e = blockObstacles[obstacle][3];
        fixedObstacles.createBlockObstacle(x_s, x_e, y_s, y_e, grid);
    }
}

TEST_F(Simulation_test, test1) {
    /**
     * Set configs:
     *
     * MAX_EPISODES = 1;
     * SESSION_TIMEOUT = 10;
     * GRID_SPAN = 10
     * SWITCH_TO_EXPLOITATION_ONLY_PERCENT = 1000;
     * VR = 3
     */
    Logger::GLOBAL_LOG_LEVEL = LOG_LEVEL::DEBUG;

    player pl1(true);
    gameSimulation game(pl1.grid);
    game.player1 = &pl1;

    vector<enemy> enemies;
    enemy f1(pl1.grid, 2, 2, 1);
    enemies.push_back(f1);

    pl1.initialize(0, 0, 6, 6);
    pl1.prepareEnemiesHashMap(enemies);
    pl1.epoch = 1;
    game.learnToPlay(pl1.grid);

    auto replayBuf_ptr = pl1.getAccessToReplayMemory();
    assert (replayBuf_ptr->rewards[0] == REWARD_TRACK_FOUR_DIV);
    assert (not replayBuf_ptr->dones[0]);
    assert (replayBuf_ptr->buffer_actions[0] == ACTION_DODGE_DIAGONAL_LEFT);

    assert (replayBuf_ptr->rewards[1] == REWARD_DEATH);
    assert (replayBuf_ptr->dones[1]);
    assert (replayBuf_ptr->buffer_actions[1] == ACTION_DODGE_DIAGONAL_RIGHT);

    assert (replayBuf_ptr->idx == 2);
    assert(pl1.restoreCellX == 2 and pl1.restoreCellY == 2);

}


TEST_F(Simulation_test, test2) {
    /**
     * Set configs:
     *
     * MAX_EPISODES = 1;
     * SESSION_TIMEOUT = 5;
     * GRID_SPAN = 10
     * SWITCH_TO_EXPLOITATION_ONLY_PERCENT = 1000;
     * VR = 3
     */
    Logger::GLOBAL_LOG_LEVEL = LOG_LEVEL::DEBUG;

    player pl1(true);

    const int TOTAL_FIXED_OBSTACLES = 1;
    int blockObstacles[TOTAL_FIXED_OBSTACLES][4] = {
            //x_s, x_e, y_s, y_e
            {2, 4, 2, 3}

    };
    createAllFixedObstacles(pl1.grid, TOTAL_FIXED_OBSTACLES, blockObstacles);


    gameSimulation game(pl1.grid);
    game.player1 = &pl1;

    vector<enemy> enemies;
    enemy f1(pl1.grid, 3, 1, 1);
    enemy f2(pl1.grid, 5, 4, 2);
    enemies.push_back(f1);
    enemies.push_back(f2);

    pl1.initialize(0, 0, 6, 6);
    pl1.prepareEnemiesHashMap(enemies);
    pl1.epoch = 1;
    pl1.seedAction = 10;
    pl1.seedExplore = 20;
    game.learnToPlay(pl1.grid);

    auto replayBuf_ptr = pl1.getAccessToReplayMemory();

    assert (replayBuf_ptr->rewards[0] == REWARD_TRACK_FOUR_DIV);
    assert (not replayBuf_ptr->dones[0]);
    assert (replayBuf_ptr->buffer_actions[0] == ACTION_DODGE_DIAGONAL_RIGHT);

    assert (replayBuf_ptr->rewards[1] == REWARD_DEATH);
    assert (replayBuf_ptr->dones[1]);
    assert (replayBuf_ptr->buffer_actions[1] == ACTION_DODGE_RIGHT);

    assert (replayBuf_ptr->idx == 2);

    assert(pl1.playerDiedInPreviousEpisode);


}


TEST_F(Simulation_test, test3) {
    /// GRID SPAN = 10
    Logger::GLOBAL_LOG_LEVEL = LOG_LEVEL::DEBUG;

    player pl1(true);

    const int TOTAL_FIXED_OBSTACLES = 1;
    int blockObstacles[TOTAL_FIXED_OBSTACLES][4] = {
            //x_s, x_e, y_s, y_e
            {2, 4, 2, 3}

    };
    createAllFixedObstacles(pl1.grid, TOTAL_FIXED_OBSTACLES, blockObstacles);

    gameSimulation game(pl1.grid);
    game.player1 = &pl1;

    vector<enemy> enemies;
    enemy f1(pl1.grid, 1, 2, 1);
    enemy f2(pl1.grid, 5, 4, 2);
    enemy f3(pl1.grid, 3, 1, 3);
    enemy f4(pl1.grid, 1, 2, 4);

    enemies.push_back(f1);
    enemies.push_back(f2);
    enemies.push_back(f3);
    enemies.push_back(f4);

    pl1.initialize(0, 0, 6, 6);
    pl1.prepareEnemiesHashMap(enemies);
    game.populateEnemies(pl1.grid, true);
    game.fight(pl1.grid);

    assert(pl1.life_left > 0);

    assert(pl1.hashMapEnemies.size() == 2);
    assert (pl1.hashMapEnemies.find(2) != pl1.hashMapEnemies.end());
    assert (pl1.hashMapEnemies.find(3) != pl1.hashMapEnemies.end());
    assert(pl1.grid[1][2] == 0);

}


TEST_F(Simulation_test, test4) {

    /// GRID SPAN = 10
    Logger::GLOBAL_LOG_LEVEL = LOG_LEVEL::DEBUG;

    player pl1(true);

    const int TOTAL_FIXED_OBSTACLES = 1;
    int blockObstacles[TOTAL_FIXED_OBSTACLES][4] = {
            //x_s, x_e, y_s, y_e
            {2, 4, 2, 3}

    };
    createAllFixedObstacles(pl1.grid, TOTAL_FIXED_OBSTACLES, blockObstacles);


    gameSimulation game(pl1.grid);
    game.player1 = &pl1;

    vector<enemy> enemies;
    enemy f1(pl1.grid, 1, 2, 1);
    enemy f2(pl1.grid, 5, 4, 2);
    enemy f3(pl1.grid, 3, 1, 3);
    enemy f4(pl1.grid, 1, 2, 4);

    enemies.push_back(f1);
    enemies.push_back(f2);
    enemies.push_back(f3);
    enemies.push_back(f4);

    pl1.initialize(3, 1, 6, 6);
    pl1.prepareEnemiesHashMap(enemies);
    game.populateEnemies(pl1.grid, true);
    game.fight(pl1.grid);

    assert(pl1.life_left == 0);

    assert(pl1.hashMapEnemies.size() == 1);
    assert(pl1.hashMapEnemies.find(2) != pl1.hashMapEnemies.end());
    assert(pl1.grid[1][2] == 0);
    assert(pl1.grid[3][1] == 0);

}