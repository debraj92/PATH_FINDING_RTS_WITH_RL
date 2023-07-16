//
// Created by Debraj Ray on 2022-09-26.
//

#ifndef EXAMPLE_SIMPLEUIVIEW_H
#define EXAMPLE_SIMPLEUIVIEW_H

#include <vector>
#include <mutex>
#include "Logger.h"
#include "TileMap.h"
#include "enemy/enemy.h"
#include "enemy/enemyUIData.h"
#include <vector>

using namespace std;
class SimpleUIView {

    LOG_LEVEL LogLevel = LOG_LEVEL::INFO;
    std::unique_ptr<Logger> logger;

    const string TILES_RESOURCES = "../resources/game_tiles.png";

    /// critical resource. Needs protection
    bool isDrawn = false;
    bool isClosed = false;

    // player info
    int playerX = -1;
    int playerY = -1;
    bool isPlayerDead;
    bool playerChangedOrientation;
    bool playerOrientationLeft = false; // Left = true, Right = false => default
    bool playerReached;
    bool playerEnRoute;

    // destination info
    int destinationX;
    int destinationY;

    // source info
    int sourceX;
    int sourceY;

    // enemy info
    vector<enemyUIData> enemiesInThisRound;


    ////////////////////////////////////////

    std::mutex uiBufferSafeReadWrite;
    std::condition_variable uiConditions;

    int gameMap[GRID_SPAN * GRID_SPAN] =
            {
                    12, 12, 12, 1, 1, 1, 12, 12, 12, 1, 1, 1, 12, 12, 12, 1, 1, 1, 12, 12, 12, 1, 1, 1, 12, 12, 12,
                    12, 12, 12, 1, 1, 1, 12, 12, 12, 1, 1, 1, 12, 12, 12, 1, 1, 1, 12, 12, 12, 1, 1, 1, 12, 12, 12,
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                    10, 10, 11, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 12, 1, 1, 1, 11, 10, 10, 1, 1, 1, 0, 0, 0,
                    10, 10, 10, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 12, 12, 1, 1, 1, 11, 10, 10, 1, 1, 1, 0, 0, 0,
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                    0, 0, 0, 1, 1, 1, 10, 10, 10, 1, 1, 1, 0, 12, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 12, 11, 10,
                    0, 0, 0, 1, 1, 1, 10, 10, 10, 1, 1, 1, 0, 0, 12, 1, 1, 1, 0, 0, 0, 1, 1, 1, 11, 10, 10,
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                    13, 13, 13, 1, 1, 1, 13, 13, 13, 1, 1, 1, 0, 0, 0, 1, 1, 1, 10, 10, 10, 1, 1, 1, 11, 10, 11,
                    13, 13, 13, 1, 1, 1, 13, 13, 13, 1, 1, 1, 0, 0, 0, 1, 1, 1, 10, 10, 10, 1, 1, 1, 11, 10, 11,
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                    11, 11, 11, 1, 1, 1, 11, 11, 11, 1, 1, 1, 13, 13, 13, 1, 1, 1, 13, 13, 13, 1, 1, 1, 11, 10, 11,
                    11, 10, 11, 1, 1, 1, 10, 10, 10, 1, 1, 1, 13, 13, 13, 1, 1, 1, 13, 13, 13, 1, 1, 1, 10, 10, 10,
                    11, 11, 10, 1, 1, 1, 10, 10, 10, 1, 1, 1, 13, 13, 13, 1, 1, 1, 13, 13, 13, 1, 1, 1, 11, 10, 10
            };

    int convert2DIndexTo1D(int x, int y);
    void drawOnCanvas(TileMap &map, vector<int> &tiles);
    void updateMapWithCurrentState(vector<int> &tiles);

public:

    void initGameMap(vector<std::vector<int>> &grid);

    void startGameLoop(bool &isGameStopped);
    void populateInfo(int plX, int plY, int destX, int destY, int srcX, int srcY, int pl_life, vector<enemyUIData> &enemiesInThisRound);
};


#endif //EXAMPLE_SIMPLEUIVIEW_H
