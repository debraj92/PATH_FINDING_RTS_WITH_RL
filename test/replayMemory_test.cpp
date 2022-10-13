//
// Created by Debraj Ray on 2022-05-29.
//

#include <gtest/gtest.h>
#include "../DQN/ReplayMemory.h"
#include "../trainingMaps.h"
#include "../observation.h"
#include "../fixedobstacles.h"

class ReplayMemory_test : public ::testing::Test {

};

void createEmptyGrid(vector<std::vector<int>> &grid) {

    // initialize an empty grid
    for (int i=0; i<GRID_SPAN; i++) {
        std::vector<int> row(GRID_SPAN, 0);
        grid.push_back(row);
    }
}

void populateEnemies(vector<std::vector<int>> &grid, vector<enemy> &enemies) {
    for(enemy e: enemies) {
        grid[e.start_x][e.start_y] = e.id;
    }
}

void printFOV(float fov[FOV_WIDTH][FOV_WIDTH]) {
    for (int i=0; i<FOV_WIDTH; i++) {
        for (int j=0; j<FOV_WIDTH; j++) {
            cout<<fov[i][j]<<" ";
        }
        cout<<endl;
    }
    cout<<endl;
}

TEST(ReplayMemory_test, test_cnn) {
    vector<std::vector<int>> grid;
    vector<enemy> enemies;
    createEmptyGrid(grid);

    CNN_controller cnnController(grid);
    std::shared_ptr<findPath> fp = make_shared<findPath>(grid, 0, 0, GRID_SPAN - 1, GRID_SPAN - 1);
    fp->findPathToDestination();
    ReplayMemory r;
    trainingMaps train(false);
    train.unregisterAllCreateMapFunctions();
    train.registerCreateMapFunction(&trainingMaps::createMapUnitTesting1);
    train.generateNextMap(grid, enemies);
    populateEnemies(grid, enemies);

    observation ob_current;
    ob_current.playerX = 2;
    ob_current.playerY = 2;
    ob_current.direction = SE;
    ob_current.locateTrajectoryAndDirection(fp);
    ob_current.recordFOVForCNN(cnnController, fp);
    ob_current.actionInPreviousState = -1;

    observation ob_next;
    ob_next.playerX = 3;
    ob_next.playerY = 3;
    ob_next.direction = SE;
    ob_next.locateTrajectoryAndDirection(fp);
    ob_next.recordFOVForCNN(cnnController, fp);
    ob_next.actionInPreviousState = -1;

    r.storeExperience(ob_current, ob_next, ACTION_STRAIGHT, 1, true);
    r.storeExperience(ob_current, ob_next, ACTION_STRAIGHT, 1, true);
    r.sampleBatch(5);

    cout<<"FOV - Current"<<endl;
    printFOV(r.obstaclesFOVcurrent[0]);
    printFOV(r.enemiesFOVcurrent[0]);
    printFOV(r.pathFOVcurrent[0]);

    cout<<"FOV - Next"<<endl;
    printFOV(r.obstaclesFOVnext[0]);
    printFOV(r.enemiesFOVnext[0]);
    printFOV(r.pathFOVnext[0]);

    cout<<"Channel current obstacles"<<endl<<r.tensor_fov_channels_current[1]<<endl;
    cout<<endl;
    cout<<"Channel next obstacles"<<endl<<r.tensor_fov_channels_next[1]<<endl;

}