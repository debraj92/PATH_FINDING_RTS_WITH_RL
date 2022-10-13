//
// Created by Debraj Ray on 2022-04-29.
//

#include <gtest/gtest.h>
#include "../trainingMaps.h"
#include "../gameConstants.h"
#include "../fixedobstacles.h"
#include "../Logger.h"
#include "random"

using namespace std;

void testMap1(std::vector<std::vector<int>>& grid, std::vector<enemy>& enemies) {

    /// Grid 6X6

    const int TOTAL_FIXED_OBSTACLES = 3;
    int blockObstacles[TOTAL_FIXED_OBSTACLES][4] = {
            //x_s, x_e, y_s, y_e
            {0, 1, 0, 1},
            {4, 4, 2, 3},
            {1, 1, 4, 4}

    };
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


// Map1
TEST(TrainMap1, BasicAssertions) {
    vector<std::vector<int>> grid;
    std::vector<enemy> enemies;
    Logger logger(LOG_LEVEL::DEBUG);

    for (int i=0; i<5; i++) {
        std::vector<int> row(5, 0);
        grid.push_back(row);
    }

    trainingMaps train(false);
    train.unregisterAllCreateMapFunctions();
    train.registerCreateMapFunction(&trainingMaps::createMapUnitTesting2);
    train.generateNextMap(grid, enemies);
    logger.printBoardDebug(grid);
    int src_x, src_y, dest_x, dest_y;
    train.setSourceAndDestination(grid, src_x, src_y, dest_x, dest_y);
    assert(src_x == 4 and src_y == 1 and dest_x == 2 and dest_y == 4);

    train.testSeed = 2;
    train.setSourceAndDestination(grid, src_x, src_y, dest_x, dest_y);
    assert(src_x == 3 and src_y == 0 and dest_x == 3 and dest_y == 4);

    train.testSeed = 3;
    train.setSourceAndDestination(grid, src_x, src_y, dest_x, dest_y);
    assert(src_x == 0 and src_y == 2 and dest_x == 2 and dest_y == 4);

}

TEST(TrainMap2, BasicAssertions) {
    /// set grid span correctly - should match sizes stored in disc
    vector<std::vector<int>> grid;
    std::vector<enemy> enemies;
    Logger logger(LOG_LEVEL::DEBUG);

    for (int i=0; i<GRID_SPAN; i++) {
        std::vector<int> row(GRID_SPAN, 0);
        grid.push_back(row);
    }

    trainingMaps train(false);
    train.generateNextMap(grid, enemies);
    logger.printBoardDebug(grid);
}

