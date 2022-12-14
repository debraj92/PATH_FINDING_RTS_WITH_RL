//
// Created by Debraj Ray on 2022-12-25.
//

#include "PotentialFieldPlayerUtil.h"
#include "gameConstants.h"
#include <cmath>

double PotentialFieldPlayerUtil::evaluateAttraction(int player_x, int player_y) {

    if(player_x < 0 or player_x >= GRID_SPAN or player_y < 0 or player_y >= GRID_SPAN) {
        return 1000000;
    }

    double distanceToGoal = pow(player_x - destination_x, 2) + pow(player_y - destination_y, 2);
    return (POTENTIAL_FIELD_ATTRACTION_C * distanceToGoal) / (2 * attractionNormalizingFactor);
}

double PotentialFieldPlayerUtil::evaluateRepulsionFromEnemyAndTrail(int player_x, int player_y, int enemy_x, int enemy_y, int enemy_id) {
    logger->logDebug("Repulsion from enemy : ")->logDebug(enemy_id)->logDebug(" = ");
    double distanceToEnemy = sqrt(pow(player_x - enemy_x, 2) + pow(player_y - enemy_y, 2));
    if (distanceToEnemy >= VISION_RADIUS) {
        logger->logDebug(0)->endLineDebug();
        return 0;
    }
    double repulsion = 0;

    if (distanceToEnemy < 1) {
        repulsion = (static_cast<double>(VISION_RADIUS - 1) / VISION_RADIUS) + 1;
    } else {
        repulsion = (VISION_RADIUS - distanceToEnemy) / (distanceToEnemy * VISION_RADIUS);

    }
    auto factor = POTENTIAL_FIELD_REPULSION_ENEMY_N;
    // enemy_id = 0 implies trail
    if(not enemy_id) {
        factor = POTENTIAL_FIELD_REPULSION_TRAIL_N;
    }
    repulsion = (factor * repulsion * repulsion) / 2;
    logger->logDebug(repulsion)->endLineDebug();
    return repulsion;
}

double
PotentialFieldPlayerUtil::evaluateRepulsion(int player_x, int player_y, const unordered_map<int, enemy> &hashMapEnemies,
                                            const vector<std::vector<int>> &grid) {

    double totalRepulsion = 0;
    for(int i = player_x - VISION_RADIUS; i<=player_x + VISION_RADIUS; i++) {
        for (int j = player_y - VISION_RADIUS; j <= player_y + VISION_RADIUS; j++) {
            if (i >= 0 and i < GRID_SPAN and j >= 0 and j < GRID_SPAN and grid[i][j] != PLAYER_ID and grid[i][j] > 0) {
                if(hashMapEnemies.find(grid[i][j]) == hashMapEnemies.end()) {
                    logger->logInfo("ERROR: Cannot find enemy, id: ")->logInfo(grid[i][j])->endLineInfo();
                }
                const enemy e = hashMapEnemies.find(grid[i][j])->second;
                totalRepulsion += evaluateRepulsionFromEnemyAndTrail(player_x, player_y, e.current_x, e.current_y, e.id);
            }
        }
    }

    // calculate repulsion from potential trail
    for(const auto &it: potentialTrail) {
        totalRepulsion += evaluateRepulsionFromEnemyAndTrail(player_x, player_y, it.first, it.second, 0);
    }

    return totalRepulsion;
}

double
PotentialFieldPlayerUtil::calculatePotentialAtLocation(int player_x, int player_y, const unordered_map<int, enemy> &hashMapEnemies,
                                            const vector<std::vector<int>> &grid) {

    // obstacle or invalid location
    if(player_x < 0 or player_x >= GRID_SPAN or player_y < 0 or player_y >= GRID_SPAN or grid[player_x][player_y] < 0) {
        return 1000000;
    }

    double attraction = evaluateAttraction(player_x, player_y);
    double repulsion = evaluateRepulsion(player_x, player_y, hashMapEnemies, grid);
    logger->logDebug("attraction potential ")->logDebug(attraction)->endLineDebug();
    logger->logDebug("repulsion potential ")->logDebug(repulsion)->endLineDebug();
    return attraction + repulsion;
}

void PotentialFieldPlayerUtil::evaluateSurroundingPotentialField(int player_x, int player_y,
                                                                    const unordered_map<int, enemy> &hashMapEnemies,
                                                                    const vector<std::vector<int>> &grid) {

    attractionNormalizingFactor = pow(player_x - destination_x, 2) + pow(player_y - destination_y, 2);

    logger->logDebug("Direction : N")->endLineDebug();
    surroundingPotentialField[N] = calculatePotentialAtLocation(player_x - 1, player_y, hashMapEnemies, grid);
    lowestPotential = surroundingPotentialField[N];
    logger->logDebug("Direction : NW")->endLineDebug();
    surroundingPotentialField[NW] = calculatePotentialAtLocation(player_x - 1, player_y - 1, hashMapEnemies, grid);
    lowestPotential = surroundingPotentialField[NW] < lowestPotential ? surroundingPotentialField[NW] : lowestPotential;
    logger->logDebug("Direction : W")->endLineDebug();
    surroundingPotentialField[W] = calculatePotentialAtLocation(player_x, player_y - 1, hashMapEnemies, grid);
    lowestPotential = surroundingPotentialField[W] < lowestPotential ? surroundingPotentialField[W] : lowestPotential;
    logger->logDebug("Direction : SW")->endLineDebug();
    surroundingPotentialField[SW] = calculatePotentialAtLocation(player_x + 1, player_y - 1, hashMapEnemies, grid);
    lowestPotential = surroundingPotentialField[SW] < lowestPotential ? surroundingPotentialField[SW] : lowestPotential;
    logger->logDebug("Direction : S")->endLineDebug();
    surroundingPotentialField[S] = calculatePotentialAtLocation(player_x + 1, player_y, hashMapEnemies, grid);
    lowestPotential = surroundingPotentialField[S] < lowestPotential ? surroundingPotentialField[S] : lowestPotential;
    logger->logDebug("Direction : SE")->endLineDebug();
    surroundingPotentialField[SE] = calculatePotentialAtLocation(player_x + 1, player_y + 1, hashMapEnemies, grid);
    lowestPotential = surroundingPotentialField[SE] < lowestPotential ? surroundingPotentialField[SE] : lowestPotential;
    logger->logDebug("Direction : E")->endLineDebug();
    surroundingPotentialField[E] = calculatePotentialAtLocation(player_x, player_y + 1, hashMapEnemies, grid);
    lowestPotential = surroundingPotentialField[E] < lowestPotential ? surroundingPotentialField[E] : lowestPotential;
    logger->logDebug("Direction : NE")->endLineDebug();
    surroundingPotentialField[NE] = calculatePotentialAtLocation(player_x - 1, player_y + 1, hashMapEnemies, grid);
    lowestPotential = surroundingPotentialField[NE] < lowestPotential ? surroundingPotentialField[NE] : lowestPotential;
}

void PotentialFieldPlayerUtil::moveToLowestPotentialCell(int &player_x, int &player_y) {

    attractionNormalizingFactor = pow(player_x - destination_x, 2) + pow(player_y - destination_y, 2);
    double maxAttractionPotential = 1000000;
    int selectedDirection = 0;
    double attractionPotential;

    if(lowestPotential == surroundingPotentialField[N]) {
        maxAttractionPotential = evaluateAttraction(player_x - 1, player_y);
        selectedDirection = N;
    }

    if(lowestPotential == surroundingPotentialField[NW]) {
        attractionPotential = evaluateAttraction(player_x - 1, player_y - 1);
        if (attractionPotential < maxAttractionPotential) {
            maxAttractionPotential = attractionPotential;
            selectedDirection = NW;
        }
    }

    if(lowestPotential == surroundingPotentialField[W]) {
        attractionPotential = evaluateAttraction(player_x, player_y - 1);
        if (attractionPotential < maxAttractionPotential) {
            maxAttractionPotential = attractionPotential;
            selectedDirection = W;
        }
    }

    if(lowestPotential == surroundingPotentialField[SW]) {
        attractionPotential = evaluateAttraction(player_x + 1, player_y - 1);
        if (attractionPotential < maxAttractionPotential) {
            maxAttractionPotential = attractionPotential;
            selectedDirection = SW;
        }
    }


    if(lowestPotential == surroundingPotentialField[S]) {
        attractionPotential = evaluateAttraction(player_x + 1, player_y);
        if (attractionPotential < maxAttractionPotential) {
            maxAttractionPotential = attractionPotential;
            selectedDirection = S;
        }
    }


    if(lowestPotential == surroundingPotentialField[SE]) {
        attractionPotential = evaluateAttraction(player_x + 1, player_y + 1);
        if (attractionPotential < maxAttractionPotential) {
            maxAttractionPotential = attractionPotential;
            selectedDirection = SE;
        }
    }


    if(lowestPotential == surroundingPotentialField[E]) {
        attractionPotential = evaluateAttraction(player_x, player_y + 1);
        if (attractionPotential < maxAttractionPotential) {
            maxAttractionPotential = attractionPotential;
            selectedDirection = E;
        }
    }

    if(lowestPotential == surroundingPotentialField[NE]) {
        attractionPotential = evaluateAttraction(player_x - 1, player_y + 1);
        if (attractionPotential < maxAttractionPotential) {
            maxAttractionPotential = attractionPotential;
            selectedDirection = NE;
        }
    }
    addToPotentialTrail(player_x, player_y);

    switch (selectedDirection) {
        case N:
            logger->logDebug("Selected Direction : N")->endLineDebug();
            --player_x;
            break;
        case NW:
            logger->logDebug("Selected Direction : NW")->endLineDebug();
            --player_x;
            --player_y;
            break;
        case W:
            logger->logDebug("Selected Direction : W")->endLineDebug();
            --player_y;
            break;
        case SW:
            logger->logDebug("Selected Direction : SW")->endLineDebug();
            ++player_x;
            --player_y;
            break;
        case S:
            logger->logDebug("Selected Direction : S")->endLineDebug();
            ++player_x;
            break;
        case SE:
            logger->logDebug("Selected Direction : SE")->endLineDebug();
            ++player_x;
            ++player_y;
            break;
        case E:
            logger->logDebug("Selected Direction : E")->endLineDebug();
            ++player_y;
            break;
        case NE:
            logger->logDebug("Selected Direction : NE")->endLineDebug();
            --player_x;
            ++player_y;
            break;
    }
}

void PotentialFieldPlayerUtil::setDestination(int dest_x, int dest_y) {
    destination_x = dest_x;
    destination_y = dest_y;
}

void PotentialFieldPlayerUtil::addToPotentialTrail(int x, int y) {
    if(potentialTrail.size() == MAX_POTENTIAL_TRAIL_SIZE) {
        potentialTrail.pop_front();
    }
    potentialTrail.emplace_back(x, y);
}



