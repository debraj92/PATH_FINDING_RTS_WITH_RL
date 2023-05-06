//
// Created by Debraj Ray on 2022-09-26.
//

#include "GameController.h"
#include "GameMaps.h"
#include <thread>


void GameController::startGame(bool useDefaultMap) {
    player player1(false);
    /// Enable baseline for comparison
    //player1.enableBaseLinePlayer();
    //player1.enablePotentialFieldPlayer();

    player1.enableUI();
    player1.registerUIComponent(uiView);
    std::vector<enemy> enemies;
    vector<vector<int>> grid;
    if (!useDefaultMap) {
        for (int i=0; i<GRID_SPAN; i++) {
            std::vector<int> row(GRID_SPAN, 0);
            grid.push_back(row);
        }
        GameMaps gm;
        gm.generateNextMap(grid);
        gm.populateEnemies(grid, enemies);
        uiView.initGameMap(grid);
    }
    thread gamePlayRunner([this, &player1, &useDefaultMap, &grid, &enemies]{
        while(not isGameStopped) {
            if (useDefaultMap) {
                playGameAsynchronous(player1);
            } else {
                playGameAsyncOnWarcraftMap(player1, grid, enemies);
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });
    uiView.startGameLoop(isGameStopped);
    gamePlayRunner.join();
}

void GameController::playGameAsynchronous(player &player1) {
    int sx=0;
    int sy=0;
    int dx=0;
    int dy=0;
    vector<vector<int>> grid;
    for (int i=0; i<GRID_SPAN; i++) {
        std::vector<int> row(GRID_SPAN, 0);
        grid.push_back(row);
    }
    std::vector<enemy> enemies;
    TestResult t{};
    tm.generateNextMap(grid, enemies);
    tm.setSourceAndDestination(grid, sx, sy, dx, dy);

    player1.playGame(grid, enemies, sx, sy, dx, dy, t);

}

void GameController::playGameAsyncOnWarcraftMap(player &player1, vector<std::vector<int>> &grid, std::vector<enemy> &enemies) {
    int sx=26;
    int sy=2;
    int dx=5;
    int dy=23;

    //int dx=14;
    //int dy=24;

    TestResult t{};

    player1.playGame(grid, enemies, sx, sy, dx, dy, t);
}
