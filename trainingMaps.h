//
// Created by Debraj Ray on 2022-06-15.
//

#ifndef EXAMPLE_TRAININGMAPS_H
#define EXAMPLE_TRAININGMAPS_H

#include <vector>
#include <functional>
#include <vector>
#include "enemy/enemy.h"
#include "Logger.h"
#include <testing.h>
#include <chrono>

using namespace std;

class trainingMaps {

    const LOG_LEVEL LogLevel = LOG_LEVEL::INFO;

    std::unique_ptr<Logger> logger;

    int index = 0;

    int index_src_dest = 0;

    typedef void (trainingMaps::*createMapFuncPtr)(std::vector<std::vector<int>>&, std::vector<enemy>&);
    vector<createMapFuncPtr> gameMaps;

    void createAllFixedObstacles(std::vector<std::vector<int>>& grid, int TOTAL_FIXED_OBSTACLES, int blockObstacles[][4]);
    int selectRandomNumberInRange(int MIN, int MAX);
    static int selectRandomNumberInRange(int MIN, int MAX, long seed);

    void selectRandomEmptyCoordinateAtBorder(vector<std::vector<int>> &grid, int &side, int &X, int &Y);
    void clearMapAndEnemies(vector<std::vector<int>> &grid, std::vector<enemy>& enemies);

    int placeEnemies(std::vector<std::vector<int>>& grid, std::vector<enemy>& enemies, int startId, long randomNumber);

    bool areBoundariesClearOfEnemies(std::vector<std::vector<int>>& grid, int x, int y, int maxEnemiesInBoundaryAllowed);

public:

    trainingMaps(bool isTesting);

    void generateNextMap(std::vector<std::vector<int>>& grid, std::vector<enemy>& enemies);
    void setSourceAndDestination(vector<std::vector<int>> &grid, int &startX, int &startY, int &endX, int &endY);

    void setSourceAndDestinationRotating(int &startX, int &startY, int &endX, int &endY);
    void setSourceAndDestinationFixed(int &startX, int &startY, int &endX, int &endY);

    void serializeRandomMap(string mapId, long randomNumber);

    /// training maps
    void createMap1(std::vector<std::vector<int>>& grid, std::vector<enemy>& enemies);
    void createMap2(std::vector<std::vector<int>>& grid, std::vector<enemy>& enemies);
    void createMap3(std::vector<std::vector<int>>& grid, std::vector<enemy>& enemies);
    void createMap4(std::vector<std::vector<int>>& grid, std::vector<enemy>& enemies);
    void createMap5(std::vector<std::vector<int>>& grid, std::vector<enemy>& enemies);
    void createMap6(std::vector<std::vector<int>>& grid, std::vector<enemy>& enemies);
    void createMap7(std::vector<std::vector<int>>& grid, std::vector<enemy>& enemies);
    void createMap8(std::vector<std::vector<int>>& grid, std::vector<enemy>& enemies);
    void createMap9(std::vector<std::vector<int>>& grid, std::vector<enemy>& enemies);
    void createMap10(std::vector<std::vector<int>>& grid, std::vector<enemy>& enemies);
    void createMap11(std::vector<std::vector<int>>& grid, std::vector<enemy>& enemies);
    void createMap12(std::vector<std::vector<int>>& grid, std::vector<enemy>& enemies);
    void createMap13(std::vector<std::vector<int>>& grid, std::vector<enemy>& enemies);
    void createMap14(std::vector<std::vector<int>>& grid, std::vector<enemy>& enemies);
    void createMap15(std::vector<std::vector<int>>& grid, std::vector<enemy>& enemies);
    void createMap16(std::vector<std::vector<int>>& grid, std::vector<enemy>& enemies);


    /// testing maps
    bool isMap1Cached;
    vector<std::vector<int>> map1_cached_grid;
    std::vector<enemy> map1_cached_enemy;

    bool isMap2Cached;
    vector<std::vector<int>> map2_cached_grid;
    std::vector<enemy> map2_cached_enemy;

    bool isMap3Cached;
    vector<std::vector<int>> map3_cached_grid;
    std::vector<enemy> map3_cached_enemy;

    bool isMap4Cached;
    vector<std::vector<int>> map4_cached_grid;
    std::vector<enemy> map4_cached_enemy;

    bool isMap5Cached;
    vector<std::vector<int>> map5_cached_grid;
    std::vector<enemy> map5_cached_enemy;

    bool isMap6Cached;
    vector<std::vector<int>> map6_cached_grid;
    std::vector<enemy> map6_cached_enemy;

    bool isMap7Cached;
    vector<std::vector<int>> map7_cached_grid;
    std::vector<enemy> map7_cached_enemy;

    bool isMap8Cached;
    vector<std::vector<int>> map8_cached_grid;
    std::vector<enemy> map8_cached_enemy;

    bool isMap9Cached;
    vector<std::vector<int>> map9_cached_grid;
    std::vector<enemy> map9_cached_enemy;

    bool isMap10Cached;
    vector<std::vector<int>> map10_cached_grid;
    std::vector<enemy> map10_cached_enemy;

    bool isMap11Cached;
    vector<std::vector<int>> map11_cached_grid;
    std::vector<enemy> map11_cached_enemy;

    bool isMap12Cached;
    vector<std::vector<int>> map12_cached_grid;
    std::vector<enemy> map12_cached_enemy;

    bool isMap13Cached;
    vector<std::vector<int>> map13_cached_grid;
    std::vector<enemy> map13_cached_enemy;

    bool isMap14Cached;
    vector<std::vector<int>> map14_cached_grid;
    std::vector<enemy> map14_cached_enemy;

    bool isMap15Cached;
    vector<std::vector<int>> map15_cached_grid;
    std::vector<enemy> map15_cached_enemy;

    bool isMap16Cached;
    vector<std::vector<int>> map16_cached_grid;
    std::vector<enemy> map16_cached_enemy;


#ifdef TESTING
    /// For Unit testing
    void createMapUnitTesting1(std::vector<std::vector<int>>& grid, std::vector<enemy>& enemies);
    void createMapUnitTesting2(std::vector<std::vector<int>>& grid, std::vector<enemy>& enemies);
    void createMapUnitTesting3(std::vector<std::vector<int>>& grid, std::vector<enemy>& enemies);
    void registerCreateMapFunction(const createMapFuncPtr createMapFunc);
    void unregisterAllCreateMapFunctions();

    int testSeed = 1;
#endif
};


#endif //EXAMPLE_TRAININGMAPS_H
