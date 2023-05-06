//
// Created by Debraj Ray on 2023-04-22.
//

#include "GameMaps.h"
#include "gameConstants.h"
#include "JsonParser.h"
#include "dist/json/json.h"
#include <iostream>
#include <fstream>

using namespace std;

void GameMaps::generateNextMap(vector<std::vector<int>> &grid) {
    logger->logInfo("Map: ")->logInfo(MAP_FILE)->endLineInfo();
    std::ifstream file(MAP_FILE);
    if (file.is_open()) {
        std::string line;
        /**
         * discard 4 lines
         * type, height, width, map
         */
        for(int i=0; i<4; ++i) {
            std::getline(file, line);
        }
        /**
         * Now we have to read 512 X 512 data
         */
        line = "";
        if(grid.capacity() == 0) {
            grid.resize(GRID_SPAN);
        }
        int row = 0, col = 0;
        while (std::getline(file, line)) {
            if(grid[row].capacity() == 0) {
                grid[row].resize(GRID_SPAN);
            }
            for(char x: line) {
                if(x == '.') {
                   grid[row][col] = 0;
                } else if (x == '@') {
                    grid[row][col] = -12;
                } else if (x == 'W') {
                    grid[row][col] = -10;
                } else if (x == 'T') {
                    grid[row][col] = -11;
                } else if (x == 'S') {
                    grid[row][col] = -13;
                } else {
                    grid[row][col] = -1;
                }
                ++col;
            }
            ++row;
            col = 0;
        }
        file.close();
    } else {
        throw std::runtime_error("Could not open file: " + MAP_FILE);
    }
}

AbstractGraph GameMaps::createAbstractGraph(RealWorld &rw, vector<std::vector<int>> &grid) {
    return move(AbstractGraph(rw, 4));
}

void GameMaps::populateEnemies(vector<std::vector<int>> &grid, vector<enemy> &enemies) {
    logger->logInfo("Populating Enemies")->endLineInfo();
    Json::Value enemies_(Json::arrayValue);
    std::ifstream enemy_file(ENEMY_FILE);
    enemy_file >> enemies_;
    const int TOTAL_ENEMIES = enemies_.size();
    int enemyLocations[TOTAL_ENEMIES][2];
    cout<<"Number of enemies "<<TOTAL_ENEMIES<<endl;
    for(int i=0; i < TOTAL_ENEMIES; ++i) {
        enemyLocations[i][0] = enemies_.get(i, Json::Value("X"))["enemy"]
                .get(Json::ArrayIndex(0), Json::Value("X")).asInt();
        enemyLocations[i][1] = enemies_.get(i, Json::Value("X"))["enemy"]
                .get(Json::ArrayIndex(1), Json::Value("X")).asInt();
    }

    int enemy_id = 1;
    for(int i=0; i < TOTAL_ENEMIES; i++) {
        enemies.emplace_back(grid,
                             enemyLocations[i][0],
                             enemyLocations[i][1],
                             enemy_id);
        enemy_id = enemy_id + 1 == PLAYER_ID ? enemy_id + 2 : enemy_id + 1;
    }
}

RealWorld GameMaps::createRealWorld(vector<std::vector<int>> &grid) {
    RealWorld rw;
    rw.loadMap(grid);
    return move(rw);
}

void GameMaps::serializeEnemies(vector<std::vector<int>> &grid, vector<enemy> &enemies) {
    logger->logInfo("Serializing Enemies")->endLineInfo();
    auto rw = createRealWorld(grid);
    auto abG = createAbstractGraph(rw, grid);
    abG.createAbstractGraph();
    auto vec_abNodes = abG.getAllAbstractNodes();
    int enemy_id = 1;
    for(const auto& abNode: vec_abNodes) {
        auto x = abNode.centroidRealNode.first;
        auto y = abNode.centroidRealNode.second;
        if (grid[x][y] == 0) {
            enemies.emplace_back(grid, x, y, enemy_id);
            enemy_id = enemy_id + 1 == PLAYER_ID ? enemy_id + 2 : enemy_id + 1;
        }
    }
    Json::Value allEnemies(Json::arrayValue);
    for(int i = 0; i < enemies.size(); ++i)
    {
        Json::Value enemy_location;
        Json::Value enemy_attributes(Json::arrayValue);

        enemy_attributes.append(Json::Value(enemies[i].start_x));
        enemy_attributes.append(Json::Value(enemies[i].start_y));

        enemy_location["enemy"] = enemy_attributes;
        allEnemies.append(enemy_location);
    }
    ofstream fileEnemies;
    fileEnemies.open(ENEMY_FILE);
    Json::StyledWriter styledWriter;
    fileEnemies << styledWriter.write(allEnemies);
    fileEnemies.close();

}
