//
// Created by Debraj Ray on 2021-11-14.
//

#include "fixedobstacles.h"
#include "gameConstants.h"


void FixedObstacles::createBlockObstacle(int x_start, int x_end, int y_start, int y_end,
                                         std::vector<std::vector<int>> &grid) {
    for(int row = x_start; row <= x_end; row++) {
        for(int col = y_start; col <= y_end; col++) {
            grid[row][col] = -1;
        }
    }
}
