//
// Created by Debraj Ray on 2021-12-30.
//

#ifndef EXAMPLE_FINDPATH_H
#define EXAMPLE_FINDPATH_H

#include <iostream>
#include "vector"
#include <set>
#include <map>
#include "gameConstants.h"
#include "AStar_.h"
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

    /// Real World
    AStar_ aStar;

    /// Abstract World
    std::unique_ptr<AStar_> aStarAbs;
    vector<std::vector<int>> gridAbstract;

    int source_x;
    int source_y;
    int destination_x;
    int destination_y;
    node_ destAbsCenter;

    node_ nextAbstractCenterSaved;
    bool isAbstractSearchInUse = false;

    vector<std::pair<int, int>> path;
    std::unique_ptr<Logger> logger;

    /**
     * Compare two nodes in the A* path
     * @return +1 if first > second, -1 is first < second, 0 otherwise
     */
    int compareNodeOrders(int location1_x, int location1_y, int location2_x, int location2_y);

    node_ getAbstractCenterOfCoordinate(int x, int y);
    void replenishNodesFromRealWorldAStar(int x, int y);

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
        destAbsCenter = getAbstractCenterOfCoordinate(destination_x, destination_y);
        createAbstractWorldGrid();
    }

    findPath(std::vector<std::vector<int>> &grid) : aStar(grid){
        logger = std::make_unique<Logger>(LogLevel);
        destAbsCenter = getAbstractCenterOfCoordinate(destination_x, destination_y);
        createAbstractWorldGrid();
    }

    bool findPathToDestination();
    bool findPathToDestinationDeferred();
    void calculateNextPosition();
    void getNextPositionAfterGivenLocation(int &given_x, int &given_y, int &next_x, int &next_y);
    int getNext_x();
    int getNext_y();
    bool isOnTrack(int current_x, int current_y);
    bool isOnTrackNoMemorizing(int current_x, int current_y);
    int pathDirection(int current_x, int current_y);
    int inferDirection(int x, int y, int next_x, int next_y);
    void changeSourceAndDestination(int startX, int startY, int endX, int endY);
    void changeMap(vector<vector<int>> &grid);
    int getCountOfNodesToDestination();

    void stitchNewPathIntoExistingAtNode(findPath &fp_, int xOnTrack, int yOnTrack, int newSourceX, int newSourceY);

    int getShortestDistance(int x1, int y1, int x2, int y2);

    /**
     * Abstract A*
     */
     void createAbstractWorldGrid();

    int getNodeOrder(int &x, int &y);

    void printTrack(int startX, int startY);

    void getCurrentStartOfPath(int &x, int &y);

    int getMaxMemoryUsed();

};


#endif //EXAMPLE_FINDPATH_H
