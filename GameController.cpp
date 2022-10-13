//
// Created by Debraj Ray on 2022-09-26.
//

#include "GameController.h"
#include <thread>


void GameController::startGame() {
    player player1(false);
    player1.enableUI();
    player1.registerUIComponent(uiView);
    trainingMaps tm(true);
    thread gamePlayRunner([this, &player1, &tm]{
        while(not isGameStopped) {
            playGameAsynchronous(player1, tm);
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });
    uiView.startGameLoop(isGameStopped);
    gamePlayRunner.join();
}

void GameController::playGameAsynchronous(player &player1, trainingMaps &tm) {
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

    /// Enable baseline for comparison
    //player1.enableBaseLinePlayer();

    player1.playGame(grid, enemies, sx, sy, dx, dy, t);

}
