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

using namespace std;

class GameController {

   bool isGameStopped = false;
   SimpleUIView uiView;

   void playGameAsynchronous(player &player1, trainingMaps &tm);

public:

    void startGame();
};


#endif //EXAMPLE_GAMECONTROLLER_H
