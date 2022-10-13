//
// Created by Debraj Ray on 2022-02-12.
//

#ifndef EXAMPLE_ASTAR__H
#define EXAMPLE_ASTAR__H

#include <vector>
#include <unordered_set>
#include "node_.h"
#include <unordered_map>
#include "iostream"
#include "Logger.h"
#include "objectLocator.h"
#include <cmath>

using namespace std;
using namespace RTS;

class AStar_ {

    const LOG_LEVEL LogLevel = LOG_LEVEL::INFO;

    std::unique_ptr<Logger> logger;

    vector<vector<int>> grid;
    pair<int, int> source;
    pair<int, int> destination;

    int countOfNodesToDestination;

    unordered_set<node_, node_::node_Hash> closedList;

    unordered_map<node_, node_, node_::node_Hash> childParent;

    const int GRID_SPAN_;

    objectLocator locator;

    int destinationDirection_;

    bool initialized = false;

    int maxSizeOfOpenList = 0;

    void calculateAdjacentNodes(vector<pair<int, int>>& childNodes, node_& nextNode);

    bool isDestinationFound(node_ node);

    void finalizeNodeLinks();

    int reverseNodeLinks(node_& current);

    void addEdge(int nextX, int nextY, vector<pair<int, int>> &nodes);

    double computeDistance(int x, int y);

    void reset();

    float computeGCost(node_ first, node_ second);


public:

    /**
     * A star has temporary view of the map. This is done as the map is dynamic and therefore it is frozen for
     * A star. Since different units would be using this module they would populate the map with objects in their vision.
     * Such objects would not apply to other units.
     */
    AStar_(vector<vector<int>> &grid, int startX, int startY, int endX, int endY) : GRID_SPAN_(grid.size()) {
        logger = std::make_unique<Logger>(LogLevel);
        logger->logDebug(startX)->logDebug(", ")->logDebug(startY)->endLineDebug();
        std::copy(grid.begin(), grid.end(), back_inserter(this->grid));
        source = make_pair(startX, startY);
        destination = make_pair(endX, endY);
        initialized = true;
    }

    AStar_(vector<vector<int>> &grid) : GRID_SPAN_(grid.size()) {
        logger = std::make_unique<Logger>(LogLevel);
        std::copy(grid.begin(), grid.end(), back_inserter(this->grid));
    }

    float findShortestDistance(pair<int, int> src, pair<int, int> dst);

    float findShortestDistanceEuclidean(pair<int, int> src, pair<int, int> dst);

    float findShortestTime(pair<int, int> src, pair<int, int> dst);

    bool findPathToDestination();

    node_ getNextNode(node_& current);

    bool isOnPath(node_& current);

    void printTrack(node_ root);

    int getCountOfNodesToDestination();

    void changeSourceAndDestination(int startX, int startY, int endX, int endY);

    void changeMap(vector<vector<int>> &grid);

    /**
     * Compare two nodes in the A* path
     * @param first
     * @param second
     * @return +1 if first > second, -1 is first < second, 0 otherwise
     */
    int compareNodeOrders(node_ first, node_ second);

    void eraseDestinationNode();

    int getNodeOrder(node_ n);

    int getTotalDistanceToDestination();

    void blockedDestinationCoordinateAllowed();

    void unblockDestinationCoordinate();

    void orderNodeLinks(node_ root);

    bool isInitialized();

    int getMaxMemoryUsed();

    friend class findPath;
};


#endif //EXAMPLE_ASTAR__H
