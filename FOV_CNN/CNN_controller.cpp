//
// Created by Debraj Ray on 2022-06-19.
//

#include "CNN_controller.h"

void CNN_controller::populateFOVChannels (
        int currentX, int currentY, int direction, bool isOnTrack, const shared_ptr<findPath>& fp,
        float (&obstaclesFOV)[FOV_WIDTH][FOV_WIDTH],
        float (&enemiesFOV)[FOV_WIDTH][FOV_WIDTH],
        float (&pathFOV)[FOV_WIDTH][FOV_WIDTH]) {

    enemyIds.clear();

    if(direction == 0) {
        for(int i=0; i<FOV_WIDTH; i++) {
            for (int j = 0; j < FOV_WIDTH; j++) {
                obstaclesFOV[i][j] = 0;
                enemiesFOV[i][j] = 0;
                pathFOV[i][j] = 0;
            }
        }
        return;
    }

    float min = 100;

    for(int i=0; i<FOV_WIDTH; i++) {
        for(int j=0; j<FOV_WIDTH; j++) {
            auto relativeCoordinates = fovFilters.direction_mapping[direction][i][j];
            int gridX = currentX - VISION_RADIUS + relativeCoordinates.x;
            int gridY = currentY - VISION_RADIUS + relativeCoordinates.y;
            populateFOVChannelsForLocation(gridX, gridY, i, j, fp, obstaclesFOV, enemiesFOV, pathFOV, min);
        }
    }

    if (isOnTrack) {
        pathFOV[VISION_RADIUS][VISION_RADIUS] = --min;
    }

    min--;
    // Normalizing
    if (min < 100 and min > 0) {
        for (int i = 0; i<FOV_WIDTH; i++) {
            for (int j = 0; j<FOV_WIDTH; j++) {
                if (pathFOV[i][j] >= 1) {
                    pathFOV[i][j] -= min;
                }
            }
        }
    }
}

void
CNN_controller::populateFOVChannelsForLocation(int grid_x, int grid_y, int fov_x, int fov_y, const shared_ptr<findPath>& fp,
                                               float (&obstaclesFOV)[FOV_WIDTH][FOV_WIDTH],
                                               float (&enemiesFOV)[FOV_WIDTH][FOV_WIDTH],
                                               float (&pathFOV)[FOV_WIDTH][FOV_WIDTH],
                                               float &min) {
    if(grid_x < 0 or grid_x >= GRID_SPAN or grid_y < 0 or grid_y >= GRID_SPAN) {
        // location unavailable
        obstaclesFOV[fov_x][fov_y] = 1;
        enemiesFOV[fov_x][fov_y] = 0;
        pathFOV[fov_x][fov_y] = 0;
    } else {
        obstaclesFOV[fov_x][fov_y] = grid[grid_x][grid_y] < 0 ? 1 : 0;
        if (grid[grid_x][grid_y] > 0 and grid[grid_x][grid_y] != PLAYER_ID) {
            enemiesFOV[fov_x][fov_y] = 1;
            enemyIds.push_back(grid[grid_x][grid_y]);
        } else {
            enemiesFOV[fov_x][fov_y] = 0;
        }
        if (fp->isOnTrackNoMemorizing(grid_x, grid_y)) {
            pathFOV[fov_x][fov_y] = static_cast<float>(fp->getNodeOrder(grid_x, grid_y));
            if (pathFOV[fov_x][fov_y] >= 1 and pathFOV[fov_x][fov_y] < min) {
                min = pathFOV[fov_x][fov_y];
            }
        } else {
            pathFOV[fov_x][fov_y] = 0;
        }
    }
}
