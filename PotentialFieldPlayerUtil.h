//
// Created by Debraj Ray on 2022-12-25.
//

#ifndef EXAMPLE_POTENTIALFIELDPLAYERUTIL_H
#define EXAMPLE_POTENTIALFIELDPLAYERUTIL_H

#include "enemy/enemy.h"
#include "Logger.h"
#include <vector>
#include <deque>

using namespace std;

class PotentialFieldPlayerUtil {

    const LOG_LEVEL LogLevel = LOG_LEVEL::INFO;

    int destination_x;
    int destination_y;

    std::unique_ptr<Logger> logger;

    vector<double> surroundingPotentialField;
    double lowestPotential = 0;
    double attractionNormalizingFactor = 0;

    deque<pair<int, int>> potentialTrail;

    double evaluateAttraction(int player_x, int player_y);
    double evaluateRepulsionFromEnemyAndTrail(int player_x, int player_y, int enemy_x, int enemy_y, int enemy_id);
    double evaluateRepulsion(int player_x, int player_y, const unordered_map<int, enemy> &hashMapEnemies, const vector<std::vector<int>> &grid);

    void addToPotentialTrail(int x, int y);

public:
    PotentialFieldPlayerUtil() {
        logger = std::make_unique<Logger>(LogLevel);
        surroundingPotentialField = vector<double> (9);
    }

    void setDestination(int dest_x, int dest_y);

    double calculatePotentialAtLocation(int player_x, int player_y, const unordered_map<int, enemy> &hashMapEnemies, const vector<std::vector<int>> &grid);
    void evaluateSurroundingPotentialField(int player_x, int player_y, const unordered_map<int, enemy> &hashMapEnemies, const vector<std::vector<int>> &grid);
    void moveToLowestPotentialCell(int &player_x, int &player_y);

};


#endif //EXAMPLE_POTENTIALFIELDPLAYERUTIL_H
