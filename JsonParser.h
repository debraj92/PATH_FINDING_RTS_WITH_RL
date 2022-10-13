//
// Created by Debraj Ray on 2022-07-17.
//

#ifndef EXAMPLE_JSONPARSER_H
#define EXAMPLE_JSONPARSER_H

#include <string>
#include <utility>
#include "enemy/enemy.h"

using namespace std;

class JsonParser {

    string trainingMapId;
    const string TRAINING_MAP_DIR_OBSTACLES = "/Users/debrajray/MyComputer/RL-A-STAR-THESIS/training_maps/obstacles/";
    const string TRAINING_MAP_DIR_ENEMIES = "/Users/debrajray/MyComputer/RL-A-STAR-THESIS/training_maps/enemies/";

    void openFile(const string &filePath, ofstream &file);

    void closeFile(ofstream &file);

    void createAllFixedObstacles(std::vector<std::vector<int>>& grid, int TOTAL_FIXED_OBSTACLES, int blockObstacles[][4]);

    void createEnemiesAtLocations(std::vector<std::vector<int>>& grid, std::vector<enemy>& enemies, int TOTAL_ENEMIES, int enemyLocations[][3]);

public:
    JsonParser(string trainingMapId_) : trainingMapId(std::move(trainingMapId_)) {

    }
    void serializeObstacles(int TOTAL_FIXED_OBSTACLES, int (*blockObstacles)[4]);

    void serializeEnemies(int TOTAL_ENEMIES, int (*enemyLocations)[3]);

    void readFromFileObstacles(std::vector<std::vector<int>>& grid);

    void readFromFileEnemies(std::vector<std::vector<int>>& grid, std::vector<enemy>& enemies);
};


#endif //EXAMPLE_JSONPARSER_H
