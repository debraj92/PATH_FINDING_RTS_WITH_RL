//
// Created by Debraj Ray on 2021-12-30.
//

#include "gameSimulation.h"
#include <unordered_set>
#include <chrono>

using namespace std;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

void gameSimulation::play(vector<std::vector<int>> &grid) {
    logger->logDebug("gameSimulation::play")->endLineDebug();
    player1->countPathLengthToDestination(player1->current_x, player1->current_y, player1->destination_x, player1->destination_y);
    populateEnemies(grid, false);
    if (not player1->isSimpleAstarPlayer) {
        bool isPathFound = player1->findPathToDestination(player1->current_x, player1->current_y, player1->destination_x, player1->destination_y);
        if (not isPathFound) {
            logger->logInfo("No path found, ignoring navigation")->endLineInfo();
            return;
        }
    }
    grid[player1->current_x][player1->current_y] = 9;
    logger->printBoardDebug(grid);
    player1->timeStep = 1;
    int action = ACTION_STRAIGHT;
    int actionError = 0;
    int previousAction = action;
    int previousActionError = actionError;
    observation currentObservation;
    player1->observe(currentObservation, grid, action, actionError, false, 0);
    double cumulativeExecutionTime = 0;
    bool firstMove = true;
    vector<enemyUIData> enemiesInThisRound;
    populateEnemiesForUI(enemiesInThisRound);
    player1->publishOnUI(enemiesInThisRound);
    while((not isEpisodeComplete()) && player1->timeStep <= SESSION_TIMEOUT) {
        auto t1 = high_resolution_clock::now();
        logger->logDebug("Time ")->logDebug(player1->timeStep)->endLineDebug();
        logger->logDebug("player (" + to_string(player1->current_x) + ", "+to_string(player1->current_y)+")")->endLineDebug();

        // Next Action
        actionError = 0;
        action = movePlayer(grid, currentObservation, &actionError);
        bool isPathFound;
        if (actionError == NEXT_Q_TOO_LOW_ERROR) {
            // unit in really tough situation according to Q values of next states, re-try with re-route
            // block all available actions and re-route
            player1->addTemporaryObstaclesToAidReroute(currentObservation.direction, (const int[]){1, 1, 1, 1, 1});
            isPathFound = player1->findPathToDestination(player1->current_x, player1->current_y, player1->destination_x, player1->destination_y);
            player1->removeTemporaryObstacles();
            if (not isPathFound) {
                // block only straight action and re-route
                player1->addTemporaryObstaclesToAidReroute(currentObservation.direction, (const int[]){0, 0, 1, 0, 0});
                isPathFound = player1->findPathToDestination(player1->current_x, player1->current_y, player1->destination_x, player1->destination_y);
                player1->removeTemporaryObstacles();
                if (not isPathFound) {
                    logger->logInfo("No path found, re-routing will be unsuccessful")->endLineInfo();
                    if (not player1->findPathToDestination(player1->current_x, player1->current_y, player1->destination_x, player1->destination_y)) {
                        logger->logInfo("ERROR: NO PATH FOUND. USE CASE FAILED")->endLineInfo();
                    }
                }
            }
            logger->logDebug("Attempting to re-route")->endLineDebug();
            // observe again after re-routing
            int direction = currentObservation.direction;
            int isPlayerInHotPursuit = currentObservation.isPlayerInHotPursuit;
            currentObservation = observation();
            // ignore the last action. Observe with previous to last action.
            player1->observe(currentObservation, grid, previousAction, previousActionError, isPlayerInHotPursuit, direction);
            action = movePlayer(grid, currentObservation, &actionError);
        }

        previousAction = action;
        previousActionError = actionError;

        fight(grid);
        // Enemy operations
        if (player1->life_left > 0 and not isDestinationReached()) {
            moveEnemies(grid, currentObservation, player1->timeStep);
            enemiesInThisRound.clear();
            populateEnemiesForUI(enemiesInThisRound);
            fight(grid);
            markDeadEnemies(enemiesInThisRound);
        }
        logger->printBoardDebug(grid);
        player1->publishOnUI(enemiesInThisRound);
        ++player1->timeStep;

        // Recover from bad stuck state if possible
        if(player1->markVisited() >= MAX_VISITED_FOR_STUCK or isStuckAtBorder()) {
            if (player1->isSimpleAstarPlayer) {
                player1->isSimplePlayerStuckDontReroute = true;
            } else {
                if (not player1->findPathToDestination(player1->current_x, player1->current_y, player1->destination_x, player1->destination_y)) {
                    logger->logInfo("ERROR: Player stuck and recovery not possible")->endLineInfo();
                    break;
                }
            }
        } else {
            if (player1->isSimpleAstarPlayer) {
                player1->isSimplePlayerStuckDontReroute = false;
            }
        }

        // Observe next State
        observation nextObservation;
        player1->observe(nextObservation, grid, action, actionError, currentObservation.isPlayerInHotPursuit, currentObservation.direction);

        if (not player1->isSimpleAstarPlayer and nextObservation.trajectory_off_track) {
            // if unit is off-track, re-route and rescue unit.
            isPathFound = player1->findPathToDestination(player1->current_x, player1->current_y, player1->destination_x, player1->destination_y);
            if (not isPathFound) {
                logger->logInfo("No path found, ignoring navigation")->endLineInfo();
                return;
            }
            nextObservation = observation();
            player1->observe(nextObservation, grid, action, actionError, currentObservation.isPlayerInHotPursuit, currentObservation.direction);
        }
        currentObservation = nextObservation;

        auto reward = calculateReward(nextObservation, action, actionError);
        logger->logDebug("Reward received ")->logDebug(reward)->endLineDebug();

        if (currentObservation.isGoalInSight and player1->life_left > 0) {
            logger->logDebug("Marching towards destination")->endLineDebug();
            headStraightToDestination(grid, enemiesInThisRound);
        }
        auto t2 = high_resolution_clock::now();
        duration<double, std::milli> ms_double = t2 - t1;
        if (not firstMove) {
            cumulativeExecutionTime += ms_double.count();
        } else {
            firstMove = false;
        }
    }
    logger->logDebug("Player 1 life left ")->logDebug(player1->life_left)->endLineDebug();
    //logger->logInfo("Average Execution Time ")->logInfo(cumulativeExecutionTime / (player1->timeStep-1))->endLineInfo();
    //logger->logInfo("Total time steps ")->logInfo(player1->timeStep)->endLineInfo();
}



void gameSimulation::learnToPlay(std::vector<std::vector<int>> &grid) {
    logger->logDebug("gameSimulation::learnToPlay")->endLineDebug();
    populateEnemies(grid, true);
    bool isPathFound = player1->findPathToDestination(player1->current_x, player1->current_y, player1->destination_x, player1->destination_y);
    if (not isPathFound) {
        logger->logInfo("ERROR: No path to destination found, ignoring training");
        return;
    }
    grid[player1->current_x][player1->current_y] = 9;
    logger->printBoardDebug(grid);
    player1->timeStep = 1;
    observation currentObservation;
    player1->observe(currentObservation, grid, ACTION_STRAIGHT, 0, false, 0);
    //dummy
    vector<enemyUIData> enemiesInThisRound;
    while((not isEpisodeComplete()) && player1->timeStep <= SESSION_TIMEOUT) {
        logger->logDebug("Time ")->logDebug(player1->timeStep)->endLineDebug();
        logger->logDebug("player (" + to_string(player1->current_x) + ", "+to_string(player1->current_y)+")")->endLineDebug();
        int actionError = 0;
        // Next Action
        int action = movePlayer(grid, currentObservation, &actionError);
        fight(grid);
        if (player1->life_left > 0 and not isDestinationReached()) {
            moveEnemies(grid, currentObservation, player1->timeStep);
            fight(grid);
        }
        logger->printBoardDebug(grid);
        ++player1->timeStep;
        // Observe after action
        observation nextObservation;
        player1->observe(nextObservation, grid, action, actionError, currentObservation.isPlayerInHotPursuit, currentObservation.direction);
        auto reward = calculateReward(nextObservation, action, actionError);
        logger->logDebug("Reward received ")->logDebug(reward)->endLineDebug();
        player1->memorizeExperienceForReplay(currentObservation, nextObservation, action, reward, isMDPDone(nextObservation));
        currentObservation = nextObservation;
        if(currentObservation.trajectory_off_track) {
            // poisoned if off track
            player1->life_left = 0;
            logger->logDebug("Player poisoned at (")->logDebug(player1->current_x)->logDebug(",")->logDebug(player1->current_y)
                    ->logDebug(")")->endLineDebug();
        }
        player1->total_rewards += reward;
        if (currentObservation.isGoalInSight and player1->life_left > 0) {
            logger->logDebug("Marching towards destination")->endLineDebug();
            headStraightToDestination(grid, enemiesInThisRound);
        }
    }
    logger->logDebug("Player 1 life left ")->logDebug(player1->life_left)->endLineDebug();
    if (player1->life_left <= 0 and not player1->stopLearning) {
        player1->playerDiedInPreviousEpisode = player1->recordRestoreLocation();
    } else {
        player1->playerDiedInPreviousEpisode = false;
    }
}


int gameSimulation::movePlayer(vector<vector<int>> &grid, const observation &currentObservation, int* error) {

    auto savedLocationPlayerX = player1->current_x;
    auto savedLocationPlayerY = player1->current_y;
    int nextAction;
    if (player1->isSimpleAstarPlayer) {
        nextAction = ACTION_STRAIGHT;
    } else {
        nextAction = player1->selectAction(currentObservation);

        if ((*error != NEXT_Q_TOO_LOW_ERROR) and player1->isInference() and (not player1->isNextStateSafeEnough())) {
            // no point in proceeding. Need to re-route
            *error = NEXT_Q_TOO_LOW_ERROR;
            // this action will be ignored
            return -1;
        }

    }

    switch(nextAction) {
        case ACTION_STRAIGHT:
            *error = setStraightActionCoordinates(player1->current_x, player1->current_y, currentObservation.direction);
            break;
        case ACTION_DODGE_DIAGONAL_LEFT:
            *error = setDodgeDiagonalLeftActionCoordinates(player1->current_x, player1->current_y, currentObservation.direction);
            break;
        case ACTION_DODGE_DIAGONAL_RIGHT:
            *error = setDodgeDiagonalRightActionCoordinates(player1->current_x, player1->current_y, currentObservation.direction);
            break;
        case ACTION_DODGE_LEFT:
            *error = setDodgeLeftActionCoordinates(player1->current_x, player1->current_y, currentObservation.direction);
            break;
        case ACTION_DODGE_RIGHT:
            *error = setDodgeRightActionCoordinates(player1->current_x, player1->current_y, currentObservation.direction);
            break;
        default:
            logger->logInfo("ERROR: Wrong next action")->endLineInfo();
    }
    if(*error != -1) {
        player1->distanceTravelled++;
    }
    grid[savedLocationPlayerX][savedLocationPlayerY] = 0;
    grid[player1->current_x][player1->current_y] = 9;

    return nextAction;
}

void gameSimulation::moveEnemies(vector<std::vector<int>> &grid, observation &ob, int time) {
    unordered_set<int> enemiesToMoveId;
    for(int i = (ob.playerX - VISION_RADIUS); i<=(ob.playerX + VISION_RADIUS); i++) {
        for(int j = (ob.playerY - VISION_RADIUS); j<=(ob.playerY + VISION_RADIUS); j++) {
            if(i < 0 or i >= GRID_SPAN or j < 0 or j >= GRID_SPAN) continue;
            if(grid[i][j] > 0 and grid[i][j] != PLAYER_ID) {
                enemiesToMoveId.insert(grid[i][j]);
            }
        }
    }

    for (auto& enemyId : enemiesToMoveId) {
        if(player1->hashMapEnemies.find(enemyId)->second.doNextMove(time, grid, {ob.playerX, ob.playerY, 0})) {
            if(not player1->isTrainingMode) enemiesAwayFromBase.insert(enemyId);
        }
    }

    // Move to base is applicable only during inference
    if(not player1->isTrainingMode) {
        for (auto enemiesAwayFromBaseIterator = enemiesAwayFromBase.begin(); enemiesAwayFromBaseIterator != enemiesAwayFromBase.end();) {
            auto enemyId = *enemiesAwayFromBaseIterator;
            if(enemiesToMoveId.find(enemyId) == enemiesToMoveId.end()) {
                // call move to base
                if(player1->hashMapEnemies.find(enemyId) == player1->hashMapEnemies.end()
                   or player1->hashMapEnemies.find(enemyId)->second.moveToBase(grid)) {
                    ++enemiesAwayFromBaseIterator;
                    enemiesAwayFromBase.erase(enemyId);
                    continue;
                }
            }
            ++enemiesAwayFromBaseIterator;
        }
    }
}

void gameSimulation::fight(vector<std::vector<int>> &grid) {
    std::unordered_map<node_, int, node_::node_Hash> enemyLocations;
    std::unordered_map<node_, int, node_::node_Hash> enemyLocationsCopy;
    // damage player
    for (auto& enemyIterator : player1->hashMapEnemies) {
        enemy& e = enemyIterator.second;
        // ignore dead enemies
        if (e.getLifeLeft() > 0) {
            if (e.current_x == player1->current_x && e.current_y == player1->current_y) {
                logger->logDebug("Player killed at (")->logDebug(player1->current_x)->logDebug(",")->logDebug(player1->current_y)
                        ->logDebug(")")->endLineDebug();
                player1->takeDamage(e.getAttackPoints());
                // enemy is also killed
                e.takeDamage(e.getAttackPoints());
            }
            auto enemyLocation = enemyLocations.find(node_(e.current_x, e.current_y));
            auto enemyLocationCopy = enemyLocationsCopy.find(node_(e.current_x, e.current_y));
            if (enemyLocation != enemyLocations.end()) {
                enemyLocation->second++;
                enemyLocationCopy->second++;
            } else {
                enemyLocations.insert(make_pair(node_(e.current_x, e.current_y), 1));
                enemyLocationsCopy.insert(make_pair(node_(e.current_x, e.current_y), 1));
            }
        }
    }

    // fuse enemies and remove dead enemies
    for(auto enemy_iterator = player1->hashMapEnemies.begin(); enemy_iterator != player1->hashMapEnemies.end();) {
        if (enemy_iterator->second.getLifeLeft() > 0) {
            auto enemyLocation = enemyLocations.find(node_(enemy_iterator->second.current_x, enemy_iterator->second.current_y));
            if (enemyLocation->second >= 2) {
                logger->logDebug("Enemy killed, id: ")->logDebug(enemy_iterator->second.id)->endLineDebug();
                enemy_iterator->second.takeDamage(enemy_iterator->second.getAttackPoints());
                enemyLocation->second--;
            } else if(enemyLocationsCopy.find(node_(enemy_iterator->second.current_x, enemy_iterator->second.current_y))->second >= 2) {
                // fused enemy don't take damage and is reborn
                enemy_iterator->second.reborn(enemy_iterator->second.current_x, enemy_iterator->second.current_y);
                grid[enemy_iterator->second.current_x][enemy_iterator->second.current_y] = enemy_iterator->second.id;
            }
        }

        if (enemy_iterator->second.getLifeLeft() <= 0 or enemy_iterator->second.max_moves <= 0) {
            // clean up dead enemies
            grid[enemy_iterator->second.current_x][enemy_iterator->second.current_y] = 0;
            int enemyIdDelete = enemy_iterator->first;
            enemy_iterator++;
            player1->hashMapEnemies.erase(enemyIdDelete);
        } else {
            ++enemy_iterator;
        }
    }

}

float gameSimulation::calculateReward(const observation &nextObservation, int action, int action_error) {
    if(player1->life_left <= 0) {
        return REWARD_DEATH;
    }
    if (nextObservation.trajectory_off_track) {
        return REWARD_OFFTRACK;
    }
    if(action_error == -1) {
        return REWARD_ACTION_UNAVAILABLE;
    }
    if (action == ACTION_DODGE_LEFT or action == ACTION_DODGE_RIGHT) {
        return REWARD_ACTION_LR;
    }
    if (nextObservation.isPlayerInHotPursuit) {
        return REWARD_TRACK_FOUR_DIV;
    }
    if(nextObservation.trajectory == on_track) {
        return REWARD_REACH;
    } else if (nextObservation.trajectory >= lower_bound_one_deviation && nextObservation.trajectory <= upper_bound_one_deviation) {
        return REWARD_TRACK_ONE_DIV;
    } else if (nextObservation.trajectory >= lower_bound_two_deviation && nextObservation.trajectory <= upper_bound_two_deviation) {
        return REWARD_TRACK_TWO_DIV;
    } else if (nextObservation.trajectory >= lower_bound_three_deviation && nextObservation.trajectory <= upper_bound_three_deviation) {
        return REWARD_TRACK_THREE_DIV;
    } else if (nextObservation.trajectory >= lower_bound_four_deviation && nextObservation.trajectory <= upper_bound_four_deviation) {
        return REWARD_TRACK_FOUR_DIV;
    } else {
        // Placeholder - will never hit as player is dead
        return REWARD_OFFTRACK;
    }
}


float gameSimulation::getTotalRewardsCollected() {
    return player1->total_rewards;
}

void gameSimulation::removeCharacters(std::vector<std::vector<int>> &grid) {
    for(int i=0; i<GRID_SPAN; i++) {
        for(int j=0; j<GRID_SPAN; j++) {
            if(grid[i][j] > 0) {
                grid[i][j] = 0;
            }
        }
    }
}

void gameSimulation::populateEnemies(vector<std::vector<int>> &grid, bool isTrainingMode) {
    for (auto& enemyIterator : player1->hashMapEnemies) {
        auto& e = enemyIterator.second;
        if(isTrainingMode) e.unitTraining();
        if (e.getLifeLeft() > 0) {
            grid[e.current_x][e.current_y] = e.id;
        }
    }
    enemiesAwayFromBase.clear();
}

bool gameSimulation::isDestinationReached() {
    return  player1->current_x == player1->destination_x and player1->current_y == player1->destination_y;
}

bool gameSimulation::isEpisodeComplete() {
    return isDestinationReached() or player1->life_left <= 0;
}

void gameSimulation::headStraightToDestination(vector<vector<int>> &grid, vector<enemyUIData> &enemiesInThisRound) {
    /// No enemies at interface or locations within goal radius
    findPath fp(grid, player1->current_x, player1->current_y, player1->destination_x, player1->destination_y);
    bool pathFound = fp.findPathToDestination();
    if (not pathFound) {
        logger->logInfo("ERROR: Could not find path to destination")->endLineInfo();
        player1->current_x = player1->destination_x;
        player1->current_y = player1->destination_y;
        return;
    }
    fp.visited_x_onpath = player1->current_x;
    fp.visited_y_onpath = player1->current_y;
    while(not isDestinationReached()) {
        fp.calculateNextPosition();
        grid[player1->current_x][player1->current_y] = 0;
        player1->current_x = fp.getNext_x();
        player1->current_y = fp.getNext_y();
        grid[player1->current_x][player1->current_y] = 9;
        logger->printBoardDebug(grid);
        player1->publishOnUI(enemiesInThisRound);
        fp.visited_x_onpath = player1->current_x;
        fp.visited_y_onpath = player1->current_y;
    }
}

bool gameSimulation::isMDPDone(observation &nextObservation) {
    bool mdpDone = (not nextObservation.isPlayerInHotPursuit and nextObservation.trajectory_on_track)
            or nextObservation.trajectory_off_track
            or player1->life_left <= 0;
    logger->logDebug("MDPDone ")->logDebug(mdpDone)->endLineDebug();
    return mdpDone;
}

void gameSimulation::populateEnemiesForUI(vector<enemyUIData> &enemiesUI) {
    for (const auto& enemyIterator : player1->hashMapEnemies) {
        auto e = enemyIterator.second;
        enemiesUI.push_back({
            enemyIterator.first,
           e.current_x,
           e.current_y,
           e.hasChangedOrientation,
           e.isOrientationLeft,
           false
        });
    }
}

void gameSimulation::markDeadEnemies(vector<enemyUIData> &enemiesUI) {
    for(enemyUIData &eUi : enemiesUI) {
        if(player1->hashMapEnemies.find(eUi.id) == player1->hashMapEnemies.end()) {
            // enemy died in last step
            eUi.isDead = true;
        }
    }
}

bool gameSimulation::isStuckAtBorder() {
    return player1->current_x == 0
    or player1->current_y == 0
    or player1->current_x == GRID_SPAN - 1
    or player1->current_y == GRID_SPAN - 1;

}
