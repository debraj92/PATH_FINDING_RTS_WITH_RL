//
// Created by Debraj Ray on 2022-09-26.
//

#include "GameController.h"
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
    vector<GameMaps::src_dst_data> startEnds;
    std::vector<enemy> enemiesOriginal;
    GameMaps gm;
    if (!useDefaultMap) {
        for (int i=0; i<GRID_SPAN; i++) {
            std::vector<int> row(GRID_SPAN, 0);
            grid.push_back(row);
        }
        gm.generateNextMap(grid);
        gm.populateSourceDestinations(startEnds);
        gm.populateEnemies(grid, enemiesOriginal);
        uiView.initGameMap(grid);
    }
    thread gamePlayRunner([this, &player1, &useDefaultMap, &grid, &enemies, &startEnds, &gm, &enemiesOriginal]{
        while(not isGameStopped) {
            if (useDefaultMap) {
                playGameAsynchronous(player1);
            } else {
                // Game will run infinitely. Or else set isGameStopped=true
                if (gm.isEndOfSrcDst()) {
                    gm.resetSourceDestinationPointer();
                }
                auto dataSrcDst = gm.generateNextSourceAndDestination(startEnds);
                enemies.clear();
                std::copy(enemiesOriginal.begin(), enemiesOriginal.end(), back_inserter(enemies));
                playGameAsyncOnWarcraftMap(player1, grid, enemies, dataSrcDst);
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

void GameController::playGameAsyncOnWarcraftMap(player &player1, vector<std::vector<int>> &grid, std::vector<enemy> &enemies,
                                                GameMaps::src_dst_data &data) {
    TestResult t{};
    player1.playGame(grid, enemies, data.startX, data.startY, data.endX, data.endY, t);
}
