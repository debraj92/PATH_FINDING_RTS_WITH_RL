//
// Created by Debraj Ray on 2021-12-30.
//

#include "player.h"

#include <memory>
#include "gameSimulation.h"
#include "plot/pbPlots.hpp"
#include "plot/supportLib.hpp"
#include "trainingMaps.h"

#include <random>
#include <iostream>

using namespace std;

void player::takeDamage(int points) {
    if(infiniteLife) {
        damage++;
        return;
    }
    if (life_left > 0) life_left -= points;
}

void player::learnGame() {
    vector<enemy> enemies;
    trainingMaps train(false);

    gameSimulation game(grid);
    game.player1 = this;
    int src_x, src_y, dest_x, dest_y;
    bool resumed;
    float destinationCount = 0;
    float deathCount = 0;
    float inferenceCount = 0;

    /// Start training async
    stopLearning = false;
    epoch = 1;
    thread trainingRunner([this]{runTrainingAsync();});

    while (epoch <= MAX_EPISODES) {
        // pick a random source and destination
        resumed = isResuming();
        if (not resumed) {
            /// If resumed, then do not change previous episode's source and destination.
            train.generateNextMap(grid, enemies);
            train.setSourceAndDestination(grid, src_x, src_y, dest_x, dest_y);
            //train.setSourceAndDestinationRotating( src_x, src_y, dest_x, dest_y);
            //train.setSourceAndDestinationFixed( src_x, src_y, dest_x, dest_y);

            /// If resumed, then do not change enemy positions from last episode
            /// else reset enemy positions to start of game
            prepareEnemiesHashMap(enemies);
        }
        game.player1->initialize(src_x, src_y, dest_x, dest_y);

        game.learnToPlay(grid);
        if (stopLearning) {
            inferenceCount++;
        }
        if(stopLearning and game.isDestinationReached()) {
            destinationCount++;
        }
        if(stopLearning and life_left <= 0) {
            deathCount++;
        }
        logger->printBoardDebug(grid);

        game.removeCharacters(grid);
        decayEpsilon(epoch);

        /// Wake up the reader to run training
        {
            // critical
            std::unique_lock<mutex> locker(safeTrainingExploration);
            if (stopLearning) {
                logger->logInfo("Total rewards collected ")->logInfo(game.getTotalRewardsCollected())->endLineInfo();
                rewards.push_back(game.getTotalRewardsCollected());
                epoch++;
                logger->logInfo("Epoch: ")->logInfo(epoch)->endLineInfo();
                locker.unlock();
                explorationOpportunity.notify_one();
            } else {
                if (not resumed) {
                    aggregated_rewards += game.getTotalRewardsCollected();
                    count_aggregation++;
                }
                if (explorationCount >= MIN_EXPLORATION_BEFORE_TRAINING) {
                    locker.unlock();
                    explorationOpportunity.notify_one();
                }
                explorationCount++;
            }
        }
    }

    trainingRunner.join();
    // Save Model
    saveModel(DQN_MODEL_PATH);

    plotLosses();
    plotRewards(rewards);

    logger->logInfo("Destination reach %")->logInfo(destinationCount * 100 / inferenceCount)->endLineInfo();
    logger->logInfo("Death %")->logInfo(deathCount * 100 / inferenceCount)->endLineInfo();

}

void player::playGame(vector<std::vector<int>> &gridSource, vector<enemy> &enemies, int src_x, int src_y, int dest_x, int dest_y, TestResult &result) {
    copyGrid(gridSource);
    clearVisited();
    prepareEnemiesHashMap(enemies);
    gameSimulation game(grid);
    game.player1 = this;
    logger->logDebug("Source (" + to_string(src_x) +", " + to_string(src_y) + ") Destination (" + to_string(dest_x) +", " + to_string(dest_y) +")\n")
            ->endLineDebug();
    this->initialize(src_x, src_y, dest_x, dest_y);
    game.play(grid);
    result.final_x = game.player1->current_x;
    result.final_y = game.player1->current_y;
    result.destination_x = game.player1->destination_x;
    result.destination_y = game.player1->destination_y;
    result.total_rewards = game.player1->total_rewards;
    result.damage = game.player1->damage;
    result.pathRatio = (double)game.player1->distanceTravelled / (double)game.player1->pathLength;
    result.maxMemoryUsed = game.player1->maxMemoryUsed;
    game.removeCharacters(grid);
    logger->logDebug("Total rewards collected ")->logDebug(game.getTotalRewardsCollected())->endLineDebug();
}


void player::observe(observation &ob, std::vector<std::vector<int>> &grid, const int lastAction, const int actionError,
                     const bool wasPreviousStateHotPursuit, const int previousStateDirection) {

    logger->logDebug("player::observe")->endLineDebug();
    ob.playerX = this->current_x;
    ob.playerY = this->current_y;
    ob.destinationX = this->destination_x;
    ob.destinationY = this->destination_y;

    if (isSimpleAstarPlayer and (current_x != destination_x or current_y != destination_y)) {
        if(not isSimplePlayerStuckDontReroute) {
            if (not findPathToDestination(current_x, current_y, destination_x, destination_y, true)) {
                logger->logDebug("Player could not find path to destination, will wait")->endLineDebug();
            }
        } else {
            if (not findPathToDestination(current_x, current_y, destination_x, destination_y, false)) {
                logger->logInfo("Player will use no enemies path")->endLineInfo();
                findPathToDestinationWithNoEnemies(current_x, current_y, destination_x, destination_y);
            }
        }
        ob.locateTrajectoryAndDirection(fp);
        ob.locateRelativeTrajectory();
    }

    if (isPotentialFieldPlayer and (current_x != destination_x or current_y != destination_y)) {
        if(isPotentialFieldPlayerStuck) {
            // Behave like baseline A* player if stuck
            if (not findPathToDestination(current_x, current_y, destination_x, destination_y, true)) {
                if (not findPathToDestination(current_x, current_y, destination_x, destination_y, false)) {
                    logger->logInfo("ERROR: Player could not find path to destination")->endLineInfo();
                }
            }
            ob.locateTrajectoryAndDirection(fp);
            ob.locateRelativeTrajectory();
        } else {
            pfUtil.evaluateSurroundingPotentialField(current_x, current_y, hashMapEnemies, grid);
        }
    }

    ob.findDestination(isTrainingMode and not stopLearning);

    if (not isSimpleAstarPlayer and not isPotentialFieldPlayer) {

        // Is stuck, re-route. If stuck for long (RLPlayerStuckTimer), behave like baseline A*
        if (isRLPlayerStuck and not findPathToDestination(current_x, current_y, destination_x, destination_y, false)) {
        }

        ob.locateTrajectoryAndDirection(fp);
        ob.locateRelativeTrajectory();

        ob.isTrueLastActionLeftOrRight = (lastAction == ACTION_DODGE_LEFT or lastAction == ACTION_DODGE_RIGHT) ? 1 : 0;
        ob.recordFOVForCNN(cnnController, fp);

        if (ob.direction > 0) {
            ob.locateEnemies(grid, cnnController, hashMapEnemies, timeStep);
            ob.updateObstacleDistances(grid);

            // Hot Pursuit states need previous action to predict enemy movement
            if (wasPreviousStateHotPursuit and ob.isPlayerInHotPursuit and (actionError != -1)) {
                if (previousStateDirection != ob.direction) {
                    // unit has changed direction
                    ob.actionInPreviousState = rotatePreviousAction(previousStateDirection, ob.direction, lastAction);
                } else {
                    ob.actionInPreviousState = lastAction;
                }
            } else {
                ob.actionInPreviousState = -1;
            }
        }
    }
}

bool player::findPathToDestination(int src_x, int src_y, int dst_x, int dst_y, bool dontGoCloseToEnemies) {
    logger->logDebug("findPathToDestination")->endLineDebug();
    std::vector<std::vector<int>> gridTemporary;
    std::copy(grid.begin(), grid.end(), back_inserter(gridTemporary));
    populateEnemyObstacles(gridTemporary, dontGoCloseToEnemies);
    fp = std::make_shared<findPath>(gridTemporary, src_x, src_y, dst_x, dst_y);
    bool isPathFound = fp->findPathToDestinationDeferred();
    int memoryUsed = fp->getMaxMemoryUsed();
    maxMemoryUsed = memoryUsed > maxMemoryUsed ? memoryUsed : maxMemoryUsed;
    return isPathFound;
}

bool player::findPathToDestinationWithNoEnemies(int src_x, int src_y, int dst_x, int dst_y) {
    logger->logDebug("findPathToDestination")->endLineDebug();
    fp = std::make_shared<findPath>(grid, src_x, src_y, dst_x, dst_y);
    bool isPathFound = fp->findPathToDestinationDeferred();
    int memoryUsed = fp->getMaxMemoryUsed();
    maxMemoryUsed = memoryUsed > maxMemoryUsed ? memoryUsed : maxMemoryUsed;
    return isPathFound;
}

void player::countPathLengthToDestination(int src_x, int src_y, int dst_x, int dst_y) {
    std::vector<std::vector<int>> gridTemporary;
    std::copy(grid.begin(), grid.end(), back_inserter(gridTemporary));
    findPath fp_(gridTemporary, src_x, src_y, dst_x, dst_y);
    fp_.findPathToDestination();
    pathLength = fp_.getCountOfNodesToDestination();
}

bool player::findPathToKnownPointOnTrack(int src_x, int src_y) {
    logger->logDebug("findPathToKnownPointOnTrack")->endLineDebug();

    std::vector<std::vector<int>> gridTemporary;
    std::copy(grid.begin(), grid.end(), back_inserter(gridTemporary));
    populateEnemyObstacles(gridTemporary, false);

    //find next free location in the existing path
    int x = fp->knownOnTrackX;
    int y = fp->knownOnTrackY;
    if(x != destination_x or y != destination_y) {
        fp->getNextPositionAfterGivenLocation(x, y, x, y);
        while(grid[x][y] != 0) {
            fp->getNextPositionAfterGivenLocation(x, y, x, y);
        }
    }
    findPath fpTemp(gridTemporary, src_x, src_y, x, y);
    if (not fpTemp.findPathToDestination()) {
        gridTemporary.clear();
        std::copy(grid.begin(), grid.end(), back_inserter(gridTemporary));
        findPath fpTemp2(gridTemporary, src_x, src_y, x, y);
        if (not fpTemp2.findPathToDestination()) {
            logger->logInfo("ERROR: Path to point on track not found")->endLineInfo();
            return false;
        }
        fp->stitchNewPathIntoExistingAtNode(fpTemp2, x, y, src_x, src_y);
        return true;
    }
    fp->stitchNewPathIntoExistingAtNode(fpTemp, x, y, src_x, src_y);

    return true;
}

void player::initialize(int src_x, int src_y, int dest_x, int dest_y) {

    if (isResuming()) {
        current_x = restoreCellX;
        current_y = restoreCellY;
        source_x = restoreCellX;
        source_y = restoreCellY;
        resumeCount++;
    } else {
        source_x = src_x;
        source_y = src_y;
        current_x = source_x;
        current_y = source_y;
        resumeCount = resumeCount == MAX_RESUME? 0 : resumeCount;
    }
    destination_x = dest_x;
    destination_y = dest_y;
    life_left = MAX_LIFE;
    isSimplePlayerStuckDontReroute = false;
    total_rewards = 0;
    pathLength = 0;
    distanceTravelled = 0;
    damage = 0;
    maxMemoryUsed = 0;

    if(isPotentialFieldPlayer) {
        pfUtil.setDestination(dest_x, dest_y);
    }

}

int player::selectAction(const observation& currentState) {
    return RLNN_Agent::selectAction(currentState, epoch, &isExploring);
}

void player::memorizeExperienceForReplay(observation &current, observation &next, int action, float reward, bool done) {
    // Do not avoid transition into pursuit by any strategy. Otherwise player would want to loop
    auto transitionIntoHotPursuit = (not current.isPlayerInHotPursuit) and next.isPlayerInHotPursuit;
    if ((not stopLearning) and (not next.isGoalInSight) and (not transitionIntoHotPursuit)) {
        RLNN_Agent::memorizeExperienceForReplay(current, next, action, reward, done, isExploring);
    }
}

bool player::recordRestoreLocation() {
    bool isRestoreLocationSet = false;
    if (fp->visited_x_onpath != -1) {
        restoreCellX = fp->visited_x_onpath;
        restoreCellY = fp->visited_y_onpath;
    } else {
        fp->getCurrentStartOfPath(restoreCellX, restoreCellY);
    }

    int nextCellX = -1, nextCellY = -1;
    while(not isRestoreLocationSet) {
        fp->getNextPositionAfterGivenLocation(restoreCellX, restoreCellY, nextCellX, nextCellY);
        restoreCellX = nextCellX;
        restoreCellY = nextCellY;
        if ((restoreCellX == destination_x and restoreCellY == destination_y) or fp->getShortestDistance(restoreCellX, restoreCellY, destination_x, destination_y) <= GOAL_RADIUS) {
            // cannot be restored
            return false;
        }
        isRestoreLocationSet = true;
        for (const auto& enemyIterator : hashMapEnemies) {
            auto e = enemyIterator.second;
            if (e.current_x == restoreCellX and e.current_y == restoreCellY) {
                isRestoreLocationSet = false;
                break;
            }
        }
    }
    // restored
    return true;
}

void player::plotRewards(vector<double> &rewards) {

    vector<double> rewards_averaged;
    vector<double> episodes;
    int avg_window_size = max(static_cast<int>(rewards.size() / MAX_REWARD_POINTS_IN_PLOT), 1);
    for(int i=0; i<rewards.size(); i+=avg_window_size) {
        double sum = 0;
        for(int j=i; j<i+avg_window_size and j<rewards.size(); j++) {
            sum += rewards[j];
        }
        rewards_averaged.push_back(sum/avg_window_size);
        episodes.push_back(i+1);
    }


    RGBABitmapImageReference *imageReference = CreateRGBABitmapImageReference();
    StringReference *errorMessage = new StringReference();
    auto success = DrawScatterPlot(imageReference, 1000, 1000, &episodes, &rewards_averaged, errorMessage);
    if(success){
        vector<double> *pngdata = ConvertToPNG(imageReference->image);
        WriteToFile(pngdata, "/Users/debrajray/MyComputer/RL-A-STAR-THESIS/plot2/episode_rewards.png");
        DeleteImage(imageReference->image);
    }else{
        cerr << "Error: ";
        for(wchar_t c : *errorMessage->string){
            wcerr << c;
        }
        cerr << endl;
    }
}

bool player::isResuming() {
    return (not stopLearning) and playerDiedInPreviousEpisode and resumeCount < MAX_RESUME;
}

void player::createEmptyGrid(vector<std::vector<int>> &grid) {

    // initialize an empty grid
    for (int i=0; i<GRID_SPAN; i++) {
        std::vector<int> row(GRID_SPAN, 0);
        grid.push_back(row);
    }
}

void player::loadExistingModel() {
    RLNN_Agent::loadModel(DQN_MODEL_PATH);
}

void player::copyGrid(std::vector<std::vector<int>> &gridSource) {
    for (int i=0; i<GRID_SPAN; i++) {
        for (int j=0; j<GRID_SPAN; j++) {
            grid[i][j] = gridSource[i][j];
        }
    }
}


void player::runTrainingAsync() {

    for(; epoch <= MAX_EPISODES and not stopLearning; epoch++) {

        /// Let the writer thread run a few times before executing a training pass
        {
            // critical
            std::unique_lock<mutex> locker(safeTrainingExploration);
            explorationOpportunity.wait(locker);
            if (stopLearning) {
                break;
            }
            rewards.push_back(aggregated_rewards/count_aggregation);
            explorationCount = 0;
            aggregated_rewards = 0;
            count_aggregation = 0;
        }
        if (epoch % dqnTargetUpdateNextEpisode == 0) {
            float percent_complete = (static_cast <float>(epoch) * 100) / static_cast <float>(MAX_EPISODES);
            if (percent_complete < 80) {
                logger->logInfo("Target net updated at episode ")->logInfo(epoch)->endLineInfo();
                updateTargetNet();
            }
        }
        logger->logInfo("Epoch: ")->logInfo(epoch)->endLineInfo();
        auto loss = RLNN_Agent::learnWithDQN();
        logger->logInfo("Network Loss: ")->logInfo(loss)->endLineInfo();

        checkPointModel(epoch);
    }
}

void player::enableBaseLinePlayer() {
    isSimpleAstarPlayer = true;
}

// only for enemies that are alerted
void player::populateEnemyObstacles(vector<std::vector<int>> &gridTemp, bool dontGoClose) {
    logger->logDebug("populateEnemyObstacles")->endLineDebug();

    for(int i = current_x - ENEMY_VISION_RADIUS; i <= current_x + ENEMY_VISION_RADIUS; i++) {
        for(int j = current_y - ENEMY_VISION_RADIUS; j <= current_y + ENEMY_VISION_RADIUS; j++) {
            if(i >= 0 and i < GRID_SPAN and j >= 0 and j < GRID_SPAN and grid[i][j] != PLAYER_ID and grid[i][j] > 0) {
                const auto &e = hashMapEnemies.find(grid[i][j])->second;
                if (dontGoClose) {
                    // surround enemy with obstacles
                    for(int x = e.current_x - 1; x <= e.current_x + 1; x++) {
                        for(int y = e.current_y - 1; y <= e.current_y + 1; y++) {
                            if(x >= 0 and x < GRID_SPAN and y >= 0 and y < GRID_SPAN
                               and gridTemp[x][y] >= 0 and gridTemp[x][y] != PLAYER_ID) {
                                gridTemp[x][y] = -e.id;
                            }
                        }
                    }
                } else {
                    gridTemp[e.current_x][e.current_y] = -e.id;
                }
            }
        }
    }
}

int player::rotatePreviousAction(int oldDirection, int newDirection, int previousAction) {
    int action = previousAction;
    /// Match action in anticlockwise direction
    for (int d = oldDirection; d != newDirection and action <= ACTION_DODGE_RIGHT; ++action) {
        d++;
        d = d == 9 ? 1 : d;
    }
    if (action <= ACTION_DODGE_RIGHT) {
        return action;
    }

    action = previousAction;
    /// Match action in clockwise direction
    for (int d = oldDirection; d != newDirection and action >= 0; --action) {
        d--;
        d = d == 0 ? 8 : d;
    }
    if (action >= 0) {
        return action;
    }

    /// hypothetical back actions
    /// Search new direction in backside of old direction
    int count = 0;
    action = previousAction;
    for (int d = oldDirection; d != newDirection; action++) {
        count++;
        d++;
        d = d == 9 ? 1 : d;
    }

    return action;
}

bool player::isNextStateSafeEnough() {
    return RLNN_Agent::getBestActionQ() > Q_REROUTE_THRESHOLD;
}

bool player::isInference() {
    return not RLNN_Agent::isTrainingMode;
}

void player::addTemporaryObstaclesToAidReroute(int direction, const int actionMask[ACTION_SPACE]) {
    coordinatesUtil coordinates(grid);
    int x = current_x;
    int y = current_y;
    if (actionMask[ACTION_STRAIGHT] and coordinates.setStraightActionCoordinates(x, y, direction) == 0 and grid[x][y] == 0) {
        grid[x][y] = NEXT_Q_TOO_LOW_ERROR;
    }

    x = current_x;
    y = current_y;
    if (actionMask[ACTION_DODGE_DIAGONAL_LEFT] and coordinates.setDodgeDiagonalLeftActionCoordinates(x, y, direction) == 0 and grid[x][y] == 0) {
        grid[x][y] = NEXT_Q_TOO_LOW_ERROR;
    }

    x = current_x;
    y = current_y;
    if (actionMask[ACTION_DODGE_DIAGONAL_RIGHT] and coordinates.setDodgeDiagonalRightActionCoordinates(x, y, direction) == 0 and grid[x][y] == 0) {
        grid[x][y] = NEXT_Q_TOO_LOW_ERROR;
    }

    x = current_x;
    y = current_y;
    if (actionMask[ACTION_DODGE_LEFT] and coordinates.setDodgeLeftActionCoordinates(x, y, direction) == 0 and grid[x][y] == 0) {
        grid[x][y] = NEXT_Q_TOO_LOW_ERROR;
    }

    x = current_x;
    y = current_y;
    if (actionMask[ACTION_DODGE_RIGHT] and coordinates.setDodgeRightActionCoordinates(x, y, direction) == 0 and grid[x][y] == 0) {
        grid[x][y] = NEXT_Q_TOO_LOW_ERROR;
    }
}

void player::removeTemporaryObstacles() {
    for(int r=current_x - 1; r<=current_x + 1; r++) {
        if(r < 0 or r >= GRID_SPAN) continue;
        for(int c=current_y - 1; c<=current_y + 1; c++) {
            if(c < 0 or c >= GRID_SPAN) continue;
            if(grid[r][c] == NEXT_Q_TOO_LOW_ERROR) grid[r][c] = 0;
        }
    }
}

void player::prepareEnemiesHashMap(vector<enemy> &enemies) {
    hashMapEnemies.clear();
    for(const enemy e: enemies) {
        hashMapEnemies.insert(std::make_pair(e.id, e));
    }
}

int player::markVisited() {
    visited[current_x][current_y]++;
    return visited[current_x][current_y];
}

void player::clearVisited() {
    for (int i=0; i<GRID_SPAN; i++) {
        for (int j=0; j<GRID_SPAN; j++) {
            visited[i][j] = 0;
        }
    }
}

void player::registerUIComponent(SimpleUIView &ui) {
    uiView = &ui;
}

void player::publishOnUI(vector<enemyUIData> &enemiesInThisRound) {
    if(not isTrainingMode and UIEnabled) {
        uiView->populateInfo(current_x, current_y, destination_x, destination_y, source_x, source_y, life_left, enemiesInThisRound);
    }
}

void player::enableUI() {
    UIEnabled = true;
}

void player::enableInfiniteLife() {
    infiniteLife = true;
}

void player::checkPointModel (int episode) {
    double percentage_complete = (double)episode * 100 / MAX_EPISODES;
    if (not isCheckPoint1Complete and (percentage_complete >= 90 and percentage_complete <= 93)) {
        saveModel(DQN_MODEL_PATH_CHKPOINT_1);
        isCheckPoint1Complete = true;
    } else if ((not isCheckPoint2Complete) and percentage_complete > 93 and percentage_complete <= 96) {
        saveModel(DQN_MODEL_PATH_CHKPOINT_2);
        isCheckPoint2Complete = true;
    }
}

void player::enablePotentialFieldPlayer() {
    isPotentialFieldPlayer = true;
}

void player::moveWithPotentialField() {
    if (isPotentialFieldPlayer) {
        pfUtil.moveToLowestPotentialCell(current_x, current_y);
    }
}




