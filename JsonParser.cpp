//
// Created by Debraj Ray on 2022-07-17.
//

#include "JsonParser.h"
#include "dist/json/json.h"
#include <iostream>
#include <iostream>
#include <fstream>
#include "fixedobstacles.h"

using namespace std;

void JsonParser::serializeObstacles(const int TOTAL_FIXED_OBSTACLES, int (*blockObstacles)[4]) {
    Json::Value allObstacles(Json::arrayValue);

    for(int i = 0; i < TOTAL_FIXED_OBSTACLES; ++i)
    {
        Json::Value obstacle;
        Json::Value obstacle_corners(Json::arrayValue);

        obstacle_corners.append(Json::Value(blockObstacles[i][0]));
        obstacle_corners.append(Json::Value(blockObstacles[i][1]));
        obstacle_corners.append(Json::Value(blockObstacles[i][2]));
        obstacle_corners.append(Json::Value(blockObstacles[i][3]));

        obstacle["obstacle"] = obstacle_corners;
        allObstacles.append(obstacle);
    }
    ofstream fileObstacles;
    openFile(TRAINING_MAP_DIR_OBSTACLES + trainingMapId + ".json", fileObstacles);
    Json::StyledWriter styledWriter;
    fileObstacles << styledWriter.write(allObstacles);
    closeFile(fileObstacles);
}

void JsonParser::serializeEnemies(const int TOTAL_ENEMIES, int (*enemyLocations)[3]) {
    Json::Value allEnemies(Json::arrayValue);
    for(int i = 0; i < TOTAL_ENEMIES; ++i)
    {
        Json::Value enemy_location;
        Json::Value enemy_attributes(Json::arrayValue);

        enemy_attributes.append(Json::Value(enemyLocations[i][0]));
        enemy_attributes.append(Json::Value(enemyLocations[i][1]));

        enemy_location["enemy"] = enemy_attributes;
        allEnemies.append(enemy_location);
    }
    ofstream fileEnemies;
    openFile(TRAINING_MAP_DIR_ENEMIES + trainingMapId + ".json", fileEnemies);
    Json::StyledWriter styledWriter;
    fileEnemies << styledWriter.write(allEnemies);
    closeFile(fileEnemies);
}

void JsonParser::openFile(const string &filePath, ofstream &file) {
    file.open(filePath);
}

void JsonParser::closeFile(ofstream &file) {
    file.close();
}

void JsonParser::readFromFileObstacles(std::vector<std::vector<int>>& grid) {
    Json::Value obstacles(Json::arrayValue);
    std::ifstream obstacles_file(TRAINING_MAP_DIR_OBSTACLES + trainingMapId + ".json");
    obstacles_file >> obstacles;
    //cout<<"Number of obstacles "<<obstacles.size()<<endl;
    int blockObstacles[obstacles.size()][4];
    for(int i=0; i < obstacles.size(); ++i) {
        blockObstacles[i][0] = obstacles.get(i, Json::Value("X"))["obstacle"]
                .get(Json::ArrayIndex(0), Json::Value("X")).asInt();
        blockObstacles[i][1] = obstacles.get(i, Json::Value("X"))["obstacle"]
                .get(Json::ArrayIndex(1), Json::Value("X")).asInt();
        blockObstacles[i][2] = obstacles.get(i, Json::Value("X"))["obstacle"]
                .get(Json::ArrayIndex(2), Json::Value("X")).asInt();
        blockObstacles[i][3] = obstacles.get(i, Json::Value("X"))["obstacle"]
                .get(Json::ArrayIndex(3), Json::Value("X")).asInt();
    }
    createAllFixedObstacles(grid, obstacles.size(), blockObstacles);
}

void JsonParser::readFromFileEnemies(std::vector<std::vector<int>>& grid, std::vector<enemy>& enemies) {
    Json::Value enemies_(Json::arrayValue);
    std::ifstream enemy_file(TRAINING_MAP_DIR_ENEMIES + trainingMapId + ".json");
    enemy_file >> enemies_;
    const int TOTAL_ENEMIES = enemies_.size();
    int enemyLocations[TOTAL_ENEMIES][3];
    //cout<<"Number of enemies "<<TOTAL_ENEMIES<<endl;
    for(int i=0; i < TOTAL_ENEMIES; ++i) {
        enemyLocations[i][0] = enemies_.get(i, Json::Value("X"))["enemy"]
                .get(Json::ArrayIndex(0), Json::Value("X")).asInt();
        enemyLocations[i][1] = enemies_.get(i, Json::Value("X"))["enemy"]
                .get(Json::ArrayIndex(1), Json::Value("X")).asInt();
    }
    createEnemiesAtLocations(grid, enemies, TOTAL_ENEMIES, enemyLocations);
}

void JsonParser::createAllFixedObstacles(std::vector<std::vector<int>>& grid, const int TOTAL_FIXED_OBSTACLES, int (*blockObstacles)[4]) {
    // fill with static obstacles
    FixedObstacles fixedObstacles;
    for(int obstacle=0; obstacle<TOTAL_FIXED_OBSTACLES; obstacle++) {
        int x_s = blockObstacles[obstacle][0];
        int x_e = blockObstacles[obstacle][1];
        int y_s = blockObstacles[obstacle][2];
        int y_e = blockObstacles[obstacle][3];
        fixedObstacles.createBlockObstacle(x_s, x_e, y_s, y_e, grid);
    }
}

void JsonParser::createEnemiesAtLocations(std::vector<std::vector<int>>& grid, vector<enemy> &enemies, const int TOTAL_ENEMIES, int (*enemyLocations)[3]) {

    int enemy_id = 1;
    for(int i=0; i < TOTAL_ENEMIES; i++) {
        enemies.emplace_back(grid,
                             enemyLocations[i][0],
                             enemyLocations[i][1],
                              enemy_id);
        enemy_id = enemy_id + 1 == PLAYER_ID ? enemy_id + 2 : enemy_id + 1;
    }
}