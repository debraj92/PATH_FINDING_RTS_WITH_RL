//
// Created by Debraj Ray on 2021-12-30.
//
#include "enemy.h"
#include "../coordinatesUtil.h"
#include "chrono"

int enemy::getAttackPoints() {
    return 10;
}

int enemy::getLifeLeft() {
    return life_left;
}

void enemy::takeDamage(int points) {
    if (life_left > 0) {
        life_left -= points;
    }
}

bool enemy::isPlayerInSight(int player_x, int player_y) {
    return max(abs(current_x - player_x), abs(current_y - player_y)) <= static_cast<float>(enemyVisionRadius);
}

bool enemy::doNextMove(const int time, vector<std::vector<int>> &grid, enemy::playerInfo pl_info) {
    logger->logDebug("enemy::doNextMove")->endLineDebug();
    if (timeStep > 0 and time <= timeStep) {
        timeStep = 0;
    }
    // moving enemies die after exhausting moves
    if (max_moves == 0) {
        life_left = 0;
        return false;
    }
    if(isPlayerInSight(pl_info.player_x, pl_info.player_y)) {
        bool wasPlayerInSightInLastTimeStep = time > 1 and ((time - timeStep) == 1);
        timeStep = time;
        int playerDistanceFromLastKnownLocation = max(abs(pl_info.player_x - lastKnownPlayerX), abs(pl_info.player_y - lastKnownPlayerY));
        if (wasPlayerInSightInLastTimeStep and lastKnownPlayerX >= 0 and playerDistanceFromLastKnownLocation == 1) {
            pl_info.player_direction = fp->inferDirection(lastKnownPlayerX, lastKnownPlayerY, pl_info.player_x, pl_info.player_y);
            lastKnownPlayerX = pl_info.player_x;
            lastKnownPlayerY = pl_info.player_y;
            predictNextPlayerLocation(grid, pl_info);
        } else {
            lastKnownPlayerX = pl_info.player_x;
            lastKnownPlayerY = pl_info.player_y;
        }
        int temp_current_x = current_x;
        int temp_current_y = current_y;
        fp->changeSourceAndDestination(current_x, current_y, pl_info.player_x, pl_info.player_y);

        logger->logDebug("Player in sight of enemy: ")->logDebug(id)->endLineDebug();
        isPathToBaseKnown = false;
        if (not fp->findPathToDestination()) {
            logger->logInfo("ERROR: path to player not found, enemy-id:")->logDebug(id)->endLineInfo();
            return false;
        }
        fp->getNextPositionAfterGivenLocation(temp_current_x, temp_current_y, temp_current_x, temp_current_y);
        if((not isTrainingInProgress) and calculateDistance(temp_current_x, temp_current_y, start_x, start_y) > MAX_ENEMY_DISTANCE_FOR_PURSUIT or
                                          (temp_current_x == agent_DestinationX && temp_current_y == agent_DestinationY)) {
            // forfeit pursuit. Wait for unit to come closer.
            // not important for RL strategies
            return false;
        }
        updateUIParams(temp_current_x, temp_current_y);

        if(grid[current_x][current_y] == id) {
            // if another unit has not already occupied this place as part of their next move
            grid[current_x][current_y] = 0;
        }
        current_x = temp_current_x;
        current_y = temp_current_y;

        grid[current_x][current_y] = id;
        max_moves--;
        return true;
    }
    return false;
}

void enemy::predictNextPlayerLocation(vector<std::vector<int>> &grid, playerInfo &pl_info) {
    logger->logDebug("predictNextPlayerLocation")->endLineDebug();
    if (pl_info.player_direction <= 0) {
        // direction was not inferred correctly
        // target current player location
        return;
    }
    coordinatesUtil cu(grid);
    int playerX = pl_info.player_x;
    int playerY = pl_info.player_y;

    // see which action of the player brings it closest to enemy. Target that location.
    int targetX = pl_info.player_x;
    int targetY = pl_info.player_y;
    double minDistanceToTarget = 1000;
    int error = cu.setStraightActionCoordinates(playerX, playerY, pl_info.player_direction);
    if (error != -1 and (playerX != current_x or playerY != current_y)) {
        if (isTrainingInProgress or calculateDistance(playerX, playerY, start_x, start_y)
        <= MAX_ENEMY_DISTANCE_FOR_PURSUIT) {
            double tempDistance = calculateDistance(current_x, current_y, playerX, playerY);
            if (tempDistance < minDistanceToTarget) {
                targetX = playerX;
                targetY = playerY;
                minDistanceToTarget = tempDistance;
            }
        }
    }

    playerX = pl_info.player_x;
    playerY = pl_info.player_y;
    error = cu.setDodgeDiagonalLeftActionCoordinates(playerX, playerY, pl_info.player_direction);
    if (error != -1 and (playerX != current_x or playerY != current_y)) {
        if (isTrainingInProgress or calculateDistance(playerX, playerY, start_x, start_y)
                                    <= MAX_ENEMY_DISTANCE_FOR_PURSUIT) {
            double tempDistance = calculateDistance(current_x, current_y, playerX, playerY);
            if (tempDistance < minDistanceToTarget) {
                targetX = playerX;
                targetY = playerY;
                minDistanceToTarget = tempDistance;
            }
        }
    }

    playerX = pl_info.player_x;
    playerY = pl_info.player_y;
    error = cu.setDodgeDiagonalRightActionCoordinates(playerX, playerY, pl_info.player_direction);
    if (error != -1 and (playerX != current_x or playerY != current_y)) {
        if (isTrainingInProgress or calculateDistance(playerX, playerY, start_x, start_y)
                                    <= MAX_ENEMY_DISTANCE_FOR_PURSUIT) {
            double tempDistance = calculateDistance(current_x, current_y, playerX, playerY);
            if (tempDistance < minDistanceToTarget) {
                targetX = playerX;
                targetY = playerY;
                minDistanceToTarget = tempDistance;
            }
        }
    }

    playerX = pl_info.player_x;
    playerY = pl_info.player_y;
    error = cu.setDodgeRightActionCoordinates(playerX, playerY, pl_info.player_direction);
    if (error != -1 and (playerX != current_x or playerY != current_y)) {
        if (isTrainingInProgress or calculateDistance(playerX, playerY, start_x, start_y)
                                    <= MAX_ENEMY_DISTANCE_FOR_PURSUIT) {
            double tempDistance = calculateDistance(current_x, current_y, playerX, playerY);
            if (tempDistance < minDistanceToTarget) {
                targetX = playerX;
                targetY = playerY;
                minDistanceToTarget = tempDistance;
            }
        }
    }

    playerX = pl_info.player_x;
    playerY = pl_info.player_y;
    error = cu.setDodgeLeftActionCoordinates(playerX, playerY, pl_info.player_direction);
    if (error != -1 and (playerX != current_x or playerY != current_y)) {
        if (isTrainingInProgress or calculateDistance(playerX, playerY, start_x, start_y)
                                    <= MAX_ENEMY_DISTANCE_FOR_PURSUIT) {
            double tempDistance = calculateDistance(current_x, current_y, playerX, playerY);
            if (tempDistance < minDistanceToTarget) {
                targetX = playerX;
                targetY = playerY;
                minDistanceToTarget = tempDistance;
            }
        }
    }
    if (minDistanceToTarget != 1000) {
        pl_info.player_x = targetX;
        pl_info.player_y = targetY;
        return;
    }
    // else target current player location
}

bool enemy::isPlayerTracked (int time) const {
    return time > 1 and timeStep > 0 and (time - timeStep) == 1 and lastKnownPlayerX >= 0;
}

double enemy::calculateDistance(int x1, int y1, int x2, int y2) const {
    double x_sqr = pow((x1 - x2),2);
    double y_sqr = pow((y1 - y2),2);
    return sqrt(x_sqr + y_sqr);
}

/**
 * Enemy moves back to base during inference. This is not important for learning
 */
bool enemy::moveToBase(vector<std::vector<int>> &grid) {
    if(isTrainingInProgress) return true;
    if(current_x != start_x or current_y != start_y) {
        if (not isPathToBaseKnown) {
            fp->changeSourceAndDestination(current_x, current_y, start_x, start_y);
            if (not fp->findPathToDestination()) {
                logger->logInfo("ERROR: path to base not found, enemy-id:")->logDebug(id)->endLineInfo();
                return false;
            }
            isPathToBaseKnown = true;
        }
        if(grid[current_x][current_y] == id) {
            // if another unit has not already occupied this place as part of their next move
            grid[current_x][current_y] = 0;
        }
        fp->getNextPositionAfterGivenLocation(current_x, current_y, current_x, current_y);
        grid[current_x][current_y] = id;

        if(current_x != start_x or current_y != start_y) {
            return false;
        }
    }
    isPathToBaseKnown = false;
    return true;

}

void enemy::unitTraining() {
    isTrainingInProgress = true;
}

void enemy::updateUIParams(int nextX, int nextY) {
    if(nextY > current_y) {
        hasChangedOrientation = true;
        isOrientationLeft = false;
    } else if (nextY < current_y) {
        hasChangedOrientation = true;
        isOrientationLeft = true;
    } else {
        hasChangedOrientation = false;
    }
}

void enemy::reborn() {
    this->life_left = 10;
    this->start_x = current_x;
    this->start_y = current_y;
    max_moves = ENEMY_MAX_MOVES;
    timeStep = 0;
    lastKnownPlayerX = -1;
    lastKnownPlayerY = -1;
    isPathToBaseKnown = false;
}

void enemy::setAgentDestination(int x, int y) {
    agent_DestinationX = x;
    agent_DestinationY = y;
}
