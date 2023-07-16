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
#include "findPath.h"

using namespace std;

/**
 * Only for warcraft maps
 */
class GameMaps {

    const LOG_LEVEL LogLevel = LOG_LEVEL::INFO;

    /*
     * Path lengths: 50-100, 100-150, 150-200, 200-250, 250-300, 300-350, 350-400, 400-450, 450-500, 500-550, 550-600
     */
    vector<int> bins;
    /*
    const string MAP_FILE = "/Users/debrajray/MyComputer/RL-A-STAR-THESIS/benchmark-maps/wc3maps512-map/thecrucible.map";
    const string ENEMY_FILE = "/Users/debrajray/MyComputer/RL-A-STAR-THESIS/benchmark-maps/wc3maps512-enemies/thecrucible.json";
    const string SRC_DST_FILE = "/Users/debrajray/MyComputer/RL-A-STAR-THESIS/benchmark-maps/wc3maps512-src_dst/thecrucible.csv";
    */
    /// Enables when running on UI

    const string MAP_FILE = "/Users/debrajray/MyComputer/AIIDE_23/wc2_game.map";
    const string ENEMY_FILE = "/Users/debrajray/MyComputer/AIIDE_23/wc2_enemy.json";
    const string SRC_DST_FILE = "/Users/debrajray/MyComputer/AIIDE_23/wc2_src_dst.json";

    /*
    const string MAP_FILE = "/Users/debrajray/MyComputer/AIIDE_23/wc2_game_2.map";
    const string ENEMY_FILE = "/Users/debrajray/MyComputer/AIIDE_23/wc2_enemy_2.json";
    const string SRC_DST_FILE = "/Users/debrajray/MyComputer/AIIDE_23/wc2_src_dst_2.json";
    */

    const int MAX_VISIT_FOR_ABSTRACT_DFS_DATA_COLLECTION = 400;
    const int MAX_BIN_SIZE = 200;
    const int MAX_DATA = 2000;
    const int MIN_LENGTH_PATH_IN_DATA = 50;

    int srcDst_iterator = -1;
    int len_srcDst;

    std::unique_ptr<Logger> logger;

    AbstractGraph createAbstractGraph(RealWorld &rw, vector<std::vector<int>> &grid);

    RealWorld createRealWorld(vector<std::vector<int>> &grid);

    int createRank(int x, int y);

    void dfsToFindDestination(vector<vector<int>> &grid, const AbstractNode &abNode, AbstractGraph &abGraph, findPath &fp, RealWorld &rw,
                              int startX, int startY, set<int> &visited);

    bool dfsToFindFreeRealCoordinateInAbstractRegion(int *result_x, int *result_y, int current_x, int current_y, int color, RealWorld &rw, int rank, set<int> &visited);

    int computeEuclideanDistance(int x1, int y1, int x2, int y2);

    void populateEnemiesOnRealMaps(RealWorld &rw, std::vector<enemy> &enemies);

    vector<int> split(string s);

public:

    struct src_dst_data {
        int startX;
        int startY;
        int endX;
        int endY;
        int pathLength;

        src_dst_data(int sx, int sy, int dx, int dy, int l) : startX(sx), startY(sy), endX(dx), endY(dy), pathLength(l) {

        }

        src_dst_data (vector<int> &tokens) {
            startX = tokens[0];
            startY = tokens[1];
            endX = tokens[2];
            endY = tokens[3];
            pathLength = tokens[4];
        }
    };

    vector<GameMaps::src_dst_data> collection;

    GameMaps() {
        logger = std::make_unique<Logger>(LogLevel);
        bins.resize(11);
    }

    void populateEnemies(vector<std::vector<int>> &grid, std::vector<enemy>& enemies);

    void populateSourceDestinations(vector<GameMaps::src_dst_data>& srcDstCollection);

    void serializeEnemies(vector<std::vector<int>> &grid, std::vector<enemy>& enemies);

    void generateNextMap(std::vector<std::vector<int>>& grid);

    src_dst_data generateNextSourceAndDestination(vector<GameMaps::src_dst_data>& srcDstCollection);

    void resetSourceDestinationPointer();

    void serializeStartAndEndPoints();

    bool areEnemiesNearby(RealWorld &rw, int x, int y);

    bool isEndOfSrcDst();
};


#endif //EXAMPLE_GAMEMAPS_H
