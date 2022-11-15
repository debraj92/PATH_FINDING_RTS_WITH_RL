//
// Created by Debraj Ray on 2022-11-12.
//

#include "CrossMoveHandler.h"

void CrossMoveHandler::addMove(CrossMoveHandler::move m) {
    movesInThisStep.push_back(m);
}

void CrossMoveHandler::clearAllMoves() {
    movesInThisStep.clear();
    unitsKilledInCrossMove.clear();
}

void CrossMoveHandler::checkForCrossMoves() {
    logger->logDebug("checkForCrossMoves")->endLineDebug();
    for (int i=0; i<(int)movesInThisStep.size() - 1; ++i) {
        bool x = i < (int)movesInThisStep.size() - 1;
        if(unitsKilledInCrossMove.count(movesInThisStep[i].id)) {
            // unit dead
            continue;
        }
        for (int j=i+1; j<movesInThisStep.size(); ++j) {
            if (movesInThisStep[i].startX == movesInThisStep[j].endX and movesInThisStep[i].startY == movesInThisStep[j].endY and
                movesInThisStep[i].endX == movesInThisStep[j].startX and movesInThisStep[i].endY == movesInThisStep[j].startY) {
                // swap places => cross
                unitsKilledInCrossMove.insert(movesInThisStep[i].id);
                unitsKilledInCrossMove.insert(movesInThisStep[j].id);
                continue;
            }
            bool isAdjacentStarts = (movesInThisStep[i].startX == movesInThisStep[j].startX
                    and abs(movesInThisStep[i].startY - movesInThisStep[j].startY) == 1)
                            or (movesInThisStep[i].startY == movesInThisStep[j].startY
                                and abs(movesInThisStep[i].startX - movesInThisStep[j].startX) == 1);

            if (not isAdjacentStarts) continue;

            bool isAdjacentEnds = (movesInThisStep[i].endX == movesInThisStep[j].endX
                                     and abs(movesInThisStep[i].endY - movesInThisStep[j].endY) == 1)
                                    or (movesInThisStep[i].endY == movesInThisStep[j].endY
                                        and abs(movesInThisStep[i].endX - movesInThisStep[j].endX) == 1);

            if (not isAdjacentEnds) continue;

            bool isXCross = (movesInThisStep[i].startX - movesInThisStep[j].startX) * (movesInThisStep[i].endX - movesInThisStep[j].endX) < 0;
            bool isYCross = (movesInThisStep[i].startY - movesInThisStep[j].startY) * (movesInThisStep[i].endY - movesInThisStep[j].endY) < 0;
            if (isXCross or isYCross) {
                unitsKilledInCrossMove.insert(movesInThisStep[i].id);
                unitsKilledInCrossMove.insert(movesInThisStep[j].id);
            }
        }
    }
}

set<int> & CrossMoveHandler::getUnitsKilledInCrossMove() {
    return unitsKilledInCrossMove;
}
