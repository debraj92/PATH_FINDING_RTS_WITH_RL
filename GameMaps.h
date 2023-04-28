//
// Created by Debraj Ray on 2023-04-22.
//

#ifndef EXAMPLE_GAMEMAPS_H
#define EXAMPLE_GAMEMAPS_H

#include <vector>
#include <functional>
#include <vector>
#include "enemy/enemy.h"
#include "PRA_Star/RealWorld.h"
#include "PRA_Star/AbstractGraph.h"
#include "Logger.h"

using namespace std;

class GameMaps {

    const LOG_LEVEL LogLevel = LOG_LEVEL::INFO;

    std::unique_ptr<Logger> logger;

    void populateEnemies(vector<std::vector<int>> &grid, std::vector<enemy>& enemies);

    AbstractGraph createAbstractGraph(RealWorld &rw, vector<std::vector<int>> &grid);

    RealWorld createRealWorld(vector<std::vector<int>> &grid);

public:

    void generateNextMap(std::vector<std::vector<int>>& grid, std::vector<enemy>& enemies);
};


#endif //EXAMPLE_GAMEMAPS_H
