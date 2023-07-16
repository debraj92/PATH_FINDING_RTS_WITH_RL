//
// Created by Debraj Ray on 2022-09-26.
//

#ifndef EXAMPLE_GAMECONTROLLER_H
#define EXAMPLE_GAMECONTROLLER_H

#include "UI/SimpleUIView.h"
#include "enemy/enemy.h"
#include "player.h"
#include "trainingMaps.h"
#include <vector>
#include "GameMaps.h"

using namespace std;

class GameController {

   bool isGameStopped = false;
   SimpleUIView uiView;
   trainingMaps tm;

   void playGameAsynchronous(player &player1);

    void playGameAsyncOnWarcraftMap(player &player1, vector<std::vector<int>> &grid, std::vector<enemy> &enemies,
                                    GameMaps::src_dst_data &startEnd);

public:

    GameController() : tm(true) {

    }
    void startGame(bool useDefaultMap);
};


#endif //EXAMPLE_GAMECONTROLLER_H
