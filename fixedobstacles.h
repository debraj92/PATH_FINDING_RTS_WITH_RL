//
// Created by Debraj Ray on 2021-11-14.
//

#ifndef EXAMPLE_FIXEDOBSTACLES_H
#define EXAMPLE_FIXEDOBSTACLES_H

#include "fixedobstacles.h"
#include <vector>

class FixedObstacles {

public:

    void createBlockObstacle(int x_start, int x_end, int y_start, int y_end, std::vector<std::vector<int>> &grid);
};

#endif //EXAMPLE_FIXEDOBSTACLES_H
