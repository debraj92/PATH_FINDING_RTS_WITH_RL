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

/**
 * Only for warcraft maps
 */
class GameMaps {

    const LOG_LEVEL LogLevel = LOG_LEVEL::INFO;


    //const string MAP_FILE = "/Users/debrajray/MyComputer/RL-A-STAR-THESIS/benchmark-maps/wc3maps512-map/duskwood.map";
    //const string ENEMY_FILE = "/Users/debrajray/MyComputer/RL-A-STAR-THESIS/benchmark-maps/wc3maps512-enemies/duskwood.json";

    /// Enables when running on UI
    const string MAP_FILE = "/Users/debrajray/MyComputer/AIIDE_23/wc2_test.map";
    const string ENEMY_FILE = "/Users/debrajray/MyComputer/AIIDE_23/wc2_test.json";

    std::unique_ptr<Logger> logger;

    AbstractGraph createAbstractGraph(RealWorld &rw, vector<std::vector<int>> &grid);

    RealWorld createRealWorld(vector<std::vector<int>> &grid);

public:

    GameMaps() {
        logger = std::make_unique<Logger>(LogLevel);
    }

    void populateEnemies(vector<std::vector<int>> &grid, std::vector<enemy>& enemies);

    void serializeEnemies(vector<std::vector<int>> &grid, std::vector<enemy>& enemies);

    void generateNextMap(std::vector<std::vector<int>>& grid);
};


#endif //EXAMPLE_GAMEMAPS_H
