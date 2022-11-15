//
// Created by Debraj Ray on 2022-11-12.
//

#ifndef EXAMPLE_CROSSMOVEHANDLER_H
#define EXAMPLE_CROSSMOVEHANDLER_H

#include <vector>
#include <set>
#include "Logger.h"

using namespace std;
class CrossMoveHandler {

    const LOG_LEVEL LogLevel = LOG_LEVEL::INFO;
    const bool SET_COMPONENT_LOG_OFF = true;

    std::unique_ptr<Logger> logger;

public:

    struct move {
        int startX;
        int startY;
        int endX;
        int endY;
        int id;

        move(int sx, int sy, int ex, int ey, int i) : startX(sx), startY(sy), endX(ex), endY(ey), id(i) {}
    };

    CrossMoveHandler() {
        logger = std::make_unique<Logger>(LogLevel);
    }

    void addMove(move m);

    void clearAllMoves();

    void checkForCrossMoves();

    set<int> & getUnitsKilledInCrossMove();

private:

    vector<move> movesInThisStep;

    set<int> unitsKilledInCrossMove;
};


#endif //EXAMPLE_CROSSMOVEHANDLER_H
