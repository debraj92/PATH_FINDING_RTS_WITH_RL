//
// Created by Debraj Ray on 2023-04-19.
//

#ifndef EXAMPLE_FINDPATHWITHPRASTAR_H
#define EXAMPLE_FINDPATHWITHPRASTAR_H

#include <iostream>
#include "vector"
#include <set>
#include <map>
#include "gameConstants.h"
#include "PRA_Star/PRAStar.h"
#include "gameConstants.h"
#include "Logger.h"

using namespace std;
using namespace RTS;

class findPath {

    const LOG_LEVEL LogLevel = LOG_LEVEL::INFO;

    int current_x;
    int current_y;
    int next_x;
    int next_y;

    PRAStar aStar;

    int source_x;
    int source_y;
    int destination_x;
    int destination_y;

    bool isAbstractSearchInUse = false;

    vector<std::pair<int, int>> path;
    std::unique_ptr<Logger> logger;

    int compareNodeOrders(int location1_x, int location1_y, int location2_x, int location2_y);

public:

    int visited_x_onpath = -1;
    int visited_y_onpath = -1;
    int knownOnTrackX = -1;
    int knownOnTrackY = -1;

    findPath(std::vector<std::vector<int>> &grid, int src_x, int src_y, int dest_x, int dest_y) : aStar(grid, src_x, src_y, dest_x, dest_y){
        logger = std::make_unique<Logger>(LogLevel);
        current_x = src_x;
        current_y = src_y;
        source_x = src_x;
        source_y = src_y;
        destination_x = dest_x;
        destination_y = dest_y;
    }

    findPath(std::vector<std::vector<int>> &grid) : aStar(grid){
            logger = std::make_unique<Logger>(LogLevel);
    }

    bool findPathToDestination();
    bool findPathToDestinationDeferred(bool earlyStop, bool useConstrainedAStar);
    void calculateNextPosition();
    void getNextPositionAfterGivenLocation(int &given_x, int &given_y, int &next_x, int &next_y);
    int getNext_x();
    int getNext_y();
    bool isOnTrack(int current_x, int current_y);
    bool isOnTrackNoMemorizing(int current_x, int current_y);
    int pathDirection(int current_x, int current_y);
    int inferDirection(int x, int y, int next_x, int next_y);

    void changeSourceAndDestination(int startX, int startY, int endX, int endY);
    void changeMap(std::vector<std::vector<int>> &grid);
    int getCountOfNodesToDestination();
    float findShortestDistance(pair<int, int> src, pair<int, int> dst);
    int getShortestDistance(int x1, int y1, int x2, int y2);
    void getCurrentStartOfPath(int &x, int &y);
    int getNodeOrder(int &x, int &y);

    int getMaxMemoryUsed();
    void createAbstractGraph();
};


#endif //EXAMPLE_FINDPATHWITHPRASTAR_H
