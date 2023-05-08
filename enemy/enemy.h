//
// Created by Debraj Ray on 2021-12-30.
//

#ifndef EXAMPLE_ENEMY_H
#define EXAMPLE_ENEMY_H

#include "../Logger.h"
#include "../findPath.h"


using namespace RTS;
using namespace std;

class enemy {

    LOG_LEVEL LogLevel = LOG_LEVEL::INFO;
    std::shared_ptr<Logger> logger;

    std::shared_ptr<findPath> fp;

    int timeStep = 0;

    bool isPathToBaseKnown = false;

    void updateUIParams(int nextX, int nextY);

public:
    const int id;

    int start_x;
    int start_y;

    // by default enemy can move to agent destination
    int agent_DestinationX = -1;
    int agent_DestinationY = -1;

    bool isTrainingInProgress = false;

    int lastKnownPlayerX = -1;
    int lastKnownPlayerY = -1;

    int max_moves = ENEMY_MAX_MOVES;
    const int enemyVisionRadius = ENEMY_VISION_RADIUS;

    /**
     * UI
     */
    int current_x;
    int current_y;
    int life_left;
    bool hasChangedOrientation;
    bool isOrientationLeft = true; // default

    struct playerInfo {
        int player_x;
        int player_y;
        int player_direction;
    };

    enemy(vector<std::vector<int>> &grid, int start_x, int start_y, int id) : id(id) {
        logger = std::make_shared<Logger>(LogLevel);
        this->start_x = start_x;
        this->start_y = start_y;
        this->current_x = start_x;
        this->current_y = start_y;
        this->life_left = 10;
        fp = std::make_shared<findPath>(grid);
    }

    // returns true if enemy actually moved
    bool doNextMove(int time, vector<std::vector<int>> &grid, playerInfo pl_info = {});
    // returns true if base is reached
    bool moveToBase(vector<std::vector<int>> &grid);
    int getAttackPoints();
    int getLifeLeft();
    void takeDamage(int points);
    bool isPlayerInSight(int player_x, int player_y);
    bool isPlayerTracked (int time) const;
    double calculateDistance(int x1, int y1, int x2, int y2) const;

    void predictNextPlayerLocation(vector<std::vector<int>> &grid, playerInfo &pl_info);
    void unitTraining();

    void setAgentDestination(int x, int y);

};


#endif //EXAMPLE_ENEMY_H
