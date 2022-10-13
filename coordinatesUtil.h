//
// Created by Debraj Ray on 2022-01-04.
//

#ifndef EXAMPLE_COORDINATESUTIL_H
#define EXAMPLE_COORDINATESUTIL_H

#include "gameConstants.h"
#include "Logger.h"
#include <vector>

using namespace std;

class coordinatesUtil {

    const LOG_LEVEL LogLevel = LOG_LEVEL::INFO;

    std::unique_ptr<Logger> logger;

    vector<vector<int>> &grid;

    bool verifyObstacle(int x, int y);

    const int GRID_SPAN_;

public:

    explicit coordinatesUtil(vector<vector<int>> &grid) : grid(grid), GRID_SPAN_(grid.size()){
        logger = std::make_unique<Logger>(LogLevel);
    }

    int setStraightActionCoordinates(int& x, int& y, int direction);
    int setDodgeLeftActionCoordinates(int& x, int& y, int direction);
    int setDodgeDiagonalLeftActionCoordinates(int& x, int& y, int direction);
    int setDodgeRightActionCoordinates(int& x, int& y, int direction);
    int setDodgeDiagonalRightActionCoordinates(int& x, int& y, int direction);

    int setNCoordinates(int& x, int& y);
    int setNECoordinates(int& x, int& y);
    int setNWCoordinates(int& x, int& y);
    int setECoordinates(int& x, int& y);
    int setWCoordinates(int& x, int& y);
    int setSCoordinates(int& x, int& y);
    int setSECoordinates(int& x, int& y);
    int setSWCoordinates(int& x, int& y);
};


#endif //EXAMPLE_COORDINATESUTIL_H
