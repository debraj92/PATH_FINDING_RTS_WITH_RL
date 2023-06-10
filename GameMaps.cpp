//
// Created by Debraj Ray on 2023-04-22.
//

#include "GameMaps.h"
#include "gameConstants.h"
#include "JsonParser.h"
#include "dist/json/json.h"
#include <iostream>
#include <fstream>
#include <random>

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
    return move(AbstractGraph(rw, 5));
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
    logger->logInfo("Done")->endLineInfo();
}

void GameMaps::serializeStartAndEndPoints() {
    logger->logInfo("serializeStartAndEndPoints")->endLineInfo();
    vector<vector<int>> grid;
    for (int i=0; i<GRID_SPAN; i++) {
        std::vector<int> row(GRID_SPAN, 0);
        grid.push_back(row);
    }
    std::vector<enemy> enemies;

    generateNextMap(grid);
    populateEnemies(grid, enemies);

    logger->logInfo("Generating Abstract Graph ")->endLineInfo();
    RealWorld rw;
    rw.loadMap(grid);
    auto ab = AbstractGraph(rw, 8);
    ab.createAbstractGraph();
    auto allAbstractNodes = ab.getAllAbstractNodes();
    std::random_device rd;
    auto rng = std::default_random_engine { rd() };
    std::shuffle(std::begin(allAbstractNodes), std::end(allAbstractNodes), rng);
    findPath fp(grid);
    logger->logInfo("Reading enemies ")->endLineInfo();
    populateEnemiesOnRealMaps(rw, enemies);
    logger->logInfo("Starting to generate start and end coordinates ")->endLineInfo();
    for(const auto& node: allAbstractNodes) {
        set<int> visitedForAbstractDfs;
        if (collection.size() > MAX_DATA) {
            break;
        }
        auto centroid = node.centroidRealNode;
        int x = centroid.first;
        int y = centroid.second;
        auto color = node.color;
        int sourceX, sourceY;
        set<int> visitedForRwDfs;
        if (dfsToFindFreeRealCoordinateInAbstractRegion(&sourceX, &sourceY, x, y, color, rw, createRank(x, y), visitedForRwDfs)) {
            assert(grid[sourceX][sourceY] == 0);
            dfsToFindDestination(grid, node, ab, fp, rw, sourceX, sourceY, visitedForAbstractDfs);
        }
    }

    logger->logInfo("Saving to file ")->endLineInfo();
    ofstream fileSrcDst;
    fileSrcDst.open(SRC_DST_FILE);
    for(auto &data: collection) {
        fileSrcDst << data.startX << "," << data.startY << "," << data.endX << "," << data.endY << "," << data.pathLength << endl;
    }
    fileSrcDst.close();
    logger->logInfo("Done")->endLineInfo();
}

void GameMaps::dfsToFindDestination(vector<vector<int>> &grid, const AbstractNode &abNode, AbstractGraph &abGraph, findPath &fp, RealWorld &rw,
                                    const int startX, const int startY, set<int> &visited) {
    if (visited.count(abNode.color) || visited.size() > MAX_VISIT_FOR_ABSTRACT_DFS_DATA_COLLECTION) {
        return;
    }
    auto centroid = abNode.centroidRealNode;
    int x = centroid.first;
    int y = centroid.second;
    auto color = abNode.color;
    visited.insert(color);
    int destX, destY;
    set<int> visitedForRwDfs;
    if (dfsToFindFreeRealCoordinateInAbstractRegion(&destX, &destY, x, y, color, rw, createRank(x, y), visitedForRwDfs)) {
        auto shortestLen = computeEuclideanDistance(startX, startY, destX, destY);
        if (shortestLen >= MIN_LENGTH_PATH_IN_DATA && bins[(int)floor(shortestLen / MIN_LENGTH_PATH_IN_DATA) - 1] < MAX_BIN_SIZE) {
            assert(grid[destX][destY] == 0);
            fp.changeSourceAndDestination(startX, startY, destX, destY);
            fp.findPathToDestination();
            src_dst_data data(startX, startY, destX, destY, fp.getCountOfNodesToDestination());
            string log = "[" + std::to_string(startX) + ", " + std::to_string(startY) + "]";
            log += "-> [" + std::to_string(destX) + ", " + std::to_string(destY) + "] ";
            log += "L " + std::to_string(fp.getCountOfNodesToDestination());
            logger->logInfo(log)->endLineInfo();
            collection.emplace_back(data);
            ++bins[(int)floor(shortestLen / MIN_LENGTH_PATH_IN_DATA) - 1];
        }
    }
    for(int adjacentNodeColor : abNode.reachableNodes) {
        auto nextNode = abGraph.unrank(adjacentNodeColor);
        dfsToFindDestination(grid, nextNode, abGraph, fp, rw, startX, startY,  visited);
    }
}

bool GameMaps::dfsToFindFreeRealCoordinateInAbstractRegion(int *result_x, int *result_y, int current_x, int current_y, const int color, RealWorld &rw, int rank, set<int> &visited) {
    if (visited.count(rank) > 0) {
        return false;
    }
    if (rw.getMapColors()[current_x][current_y] != color) {
        return false;
    }
    if (rw.getRealMap()[current_x][current_y] == 0 and !areEnemiesNearby(rw, current_x, current_y)) {
        *result_x = current_x;
        *result_y = current_y;
        return true;
    }
    visited.insert(rank);

    if( current_x > 0 ) {
        if (current_y > 0 &&
            dfsToFindFreeRealCoordinateInAbstractRegion(result_x, result_y, current_x - 1, current_y - 1, color, rw,
                                                        createRank(current_x - 1, current_y - 1), visited)) {
            return true;
        }
        if (current_y < GRID_SPAN - 1 &&
            dfsToFindFreeRealCoordinateInAbstractRegion(result_x, result_y, current_x - 1, current_y + 1, color, rw,
                                                        createRank(current_x - 1, current_y + 1), visited)) {
            return true;
        }
        if (dfsToFindFreeRealCoordinateInAbstractRegion(result_x, result_y, current_x - 1, current_y, color, rw,
                                                        createRank(current_x - 1, current_y), visited)) {
            return true;
        }
    }
    if( current_x < GRID_SPAN - 1 ) {
        if (current_y > 0 &&
            dfsToFindFreeRealCoordinateInAbstractRegion(result_x, result_y, current_x + 1, current_y - 1, color, rw,
                                                        createRank(current_x + 1, current_y - 1), visited)) {
            return true;
        }
        if (current_y < GRID_SPAN - 1 &&
            dfsToFindFreeRealCoordinateInAbstractRegion(result_x, result_y, current_x + 1, current_y + 1, color, rw,
                                                        createRank(current_x + 1, current_y + 1), visited)) {
            return true;
        }
        if (dfsToFindFreeRealCoordinateInAbstractRegion(result_x, result_y, current_x + 1, current_y, color, rw,
                                                        createRank(current_x + 1, current_y), visited)) {
            return true;
        }
    }
    if (current_y > 0 &&
        dfsToFindFreeRealCoordinateInAbstractRegion(result_x, result_y, current_x, current_y - 1, color, rw,
                                                    createRank(current_x, current_y - 1), visited)) {
        return true;
    }
    if (current_y < GRID_SPAN - 1 &&
        dfsToFindFreeRealCoordinateInAbstractRegion(result_x, result_y, current_x, current_y + 1, color, rw,
                                                    createRank(current_x, current_y + 1), visited)) {
        return true;
    }
    return false;
}

int GameMaps::createRank(int x, int y) {
    return x * 1000 + y;
}

int GameMaps::computeEuclideanDistance(int x1, int y1, int x2, int y2) {
    return (int) round(sqrtf(powf(x1 - x2, 2) + powf(y1 - y2, 2)));
}

void GameMaps::populateEnemiesOnRealMaps(RealWorld &rw, std::vector<enemy> &enemies) {
    for(const enemy &e : enemies) {
        rw.getRealMap()[e.current_x][e.current_y] = e.id;
    }
}

void GameMaps::populateSourceDestinations(vector<GameMaps::src_dst_data>& srcDstCollection) {
    logger->logInfo("Populating Source and Destinations")->endLineInfo();
    std::ifstream src_dst_file(SRC_DST_FILE);
    std::string line;
    len_srcDst = 0;
    while (std::getline(src_dst_file, line))
    {
        auto tokens = split(line);
        GameMaps::src_dst_data data(tokens);
        srcDstCollection.emplace_back(data);
        ++len_srcDst;
    }
    srcDst_iterator = 0;
}

vector<int> GameMaps::split(string s) {
    vector<int> tokens;
    stringstream ss(s);
    string token;
    while (std::getline(ss, token, ',')) {
        tokens.push_back(std::stoi(token));
    }
    return std::move(tokens);
}

GameMaps::src_dst_data GameMaps::generateNextSourceAndDestination(vector<GameMaps::src_dst_data>& srcDstCollection) {
    int pointer = srcDst_iterator;
    ++srcDst_iterator;
    return srcDstCollection[pointer];
}

bool GameMaps::areEnemiesNearby(RealWorld &rw, int x, int y) {
    for(int i = x - 2; i <= x + 2; ++i) {
        if (i < 0 or i >= GRID_SPAN) {
            continue;
        }
        for(int j = y - 2; j <= y + 2; ++j) {
            if (j < 0 or j >= GRID_SPAN) {
                continue;
            }
            if(rw.getRealMap()[i][j] > 0) return true;
        }
    }
    return false;
}

bool GameMaps::isEndOfSrcDst() {
    return srcDst_iterator >= len_srcDst;
}
