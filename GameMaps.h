//
// Created by Debraj Ray on 2023-04-22.
//

#ifndef EXAMPLE_GAMEMAPS_H
#define EXAMPLE_GAMEMAPS_H

#include <vector>
#include <functional>
#include <vector>
#include "enemy/enemy.h"
#include "Logger.h"

using namespace std;

class GameMaps {

    const LOG_LEVEL LogLevel = LOG_LEVEL::INFO;

    std::unique_ptr<Logger> logger;

public:
    void generateNextMap(std::vector<std::vector<int>>& grid, std::vector<enemy>& enemies, vector<int> &uimap);
};


#endif //EXAMPLE_GAMEMAPS_H
