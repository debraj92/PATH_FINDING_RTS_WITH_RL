//
// Created by Debraj Ray on 2022-06-19.
//

#ifndef EXAMPLE_CNN_CONTROLLER_H
#define EXAMPLE_CNN_CONTROLLER_H

#include "../gameConstants.h"
#include "../Logger.h"
#include "FOV_Filters.h"
#include <vector>
#include "../findPath.h"

using namespace std;

class CNN_controller {

    const LOG_LEVEL LogLevel = LOG_LEVEL::INFO;

    std::unique_ptr<Logger> logger;

    vector<vector<int>> &grid;

    FOV_Filters fovFilters;

    void populateFOVChannelsForLocation(int grid_x, int grid_y, int fov_x, int fov_y, const shared_ptr<findPath>& fp,
                                        float (&obstaclesFOV)[FOV_WIDTH][FOV_WIDTH],
                                        float (&enemiesFOV)[FOV_WIDTH][FOV_WIDTH],
                                        float (&pathFOV)[FOV_WIDTH][FOV_WIDTH],
                                        float &min);


public:

    CNN_controller(vector<vector<int>> &grid1) : grid(grid1) {
        logger = std::make_unique<Logger>(LogLevel);
        fovFilters.init();
    }

    // for use by caller if needed
    vector<int> enemyIds;

    void populateFOVChannels(int currentX, int currentY, int direction, bool isOnTrack, const shared_ptr<findPath>& fp,
                             float (&obstaclesFOV)[FOV_WIDTH][FOV_WIDTH],
                             float (&enemiesFOV)[FOV_WIDTH][FOV_WIDTH],
                             float (&pathFOV)[FOV_WIDTH][FOV_WIDTH]);
};


#endif //EXAMPLE_CNN_CONTROLLER_H
