//
// Created by Debraj Ray on 2023-04-19.
//

#ifndef EXAMPLE_PRASTAR_H
#define EXAMPLE_PRASTAR_H

#include "AbstractGraph.h"
#include "RealWorld.h"
#include "AStarOpenList.h"
#include "Logger.h"
#include "gameConstants.h"

using namespace std;
using namespace RTS;

class PRAStar {

    const LOG_LEVEL LogLevel = LOG_LEVEL::INFO;
    std::unique_ptr<Logger> logger;
    unique_ptr<AbstractGraph> abstractGraph;
    unique_ptr<RealWorld> realWorld;

    unique_ptr<unordered_map<node_, node_, node_::node_Hash>> pathRealWorld;
    unique_ptr<unordered_set<int>> abstractParentNodeColors;

    bool initialized = false;
    int maxSizeOfOpenList = 0;

    void insertIntoOpenList(AStarOpenList &openList,
                            unique_ptr<unordered_set<node_, node_::node_Hash>> &closedList,
                            unique_ptr<unordered_map<node_, node_, node_::node_Hash>> &childParent,
                            node_ childNode,
                            node_ &parentNode,
                            const std::function<float(node_)>& hCost,
                            float gCost);

    void finalizeNodeLinks(unique_ptr<unordered_map<node_, node_, node_::node_Hash>> &childParent,
                           unique_ptr<unordered_set<node_, node_::node_Hash>> &closedList);

    int reverseNodeLinks(node_ current, unique_ptr<unordered_map<node_, node_, node_::node_Hash>> &childParent);

    void copyRealWorld(vector<vector<int>> &copied);

    bool isDestinationFound(const node_ &node);

    void orderNodeLinks(node_ root, node_ dest);

    void eraseDestinationNode(node_ dest);

    void addEdge(int nextX, int nextY, vector<pair<int, int>> &nodes);

    /**
     * Search a path through an abstraction level. It uses constrained A* search that expands nodes whose parent node
     * is part of the abstract path in the next higher level of abstraction
     */
    bool searchPathInAbstractGraphWithAstar(unique_ptr<unordered_map<node_, node_, node_::node_Hash>> &childParent);

public:

    PRAStar(vector<vector<int>> &grid, int startX, int startY, int endX, int endY) {
        logger = std::make_unique<Logger>(LogLevel);
        logger->logDebug(startX)->logDebug(", ")->logDebug(startY)->endLineDebug();
        pathRealWorld = std::make_unique<unordered_map<node_, node_, node_::node_Hash>>();
        abstractParentNodeColors = std::make_unique<unordered_set<int>>();
        createRealWorld(grid);
        abstractGraph = std::make_unique<AbstractGraph>(*realWorld, ABSTRACT_SECTOR_PRA_STAR);
        initStartState(startX, startY);
        initGoalState(endX, endY);
        initialized = true;
    }

    PRAStar(vector<vector<int>> &grid) {
        logger = std::make_unique<Logger>(LogLevel);
        pathRealWorld = std::make_unique<unordered_map<node_, node_, node_::node_Hash>>();
        abstractParentNodeColors = std::make_unique<unordered_set<int>>();
        createRealWorld(grid);
        abstractGraph = std::make_unique<AbstractGraph>(*realWorld, ABSTRACT_SECTOR_PRA_STAR);
    }

    void createAbstractGraph();
    void createRealWorld(vector<vector<int>> &grid);

    void initStartState(int startX, int startY);
    void initGoalState(int goalX, int goalY);

    /**
     * For PRA*, provide non-empty abstractParentNodes. For A* this should be empty.
     */
    bool searchPathInRealWorldWithAstar(int parentGoal=-1);

    void resetAbstractPath();

    int getGoalColorOfAbstraction();


    void printRealPath(unique_ptr<unordered_map<node_, node_, node_::node_Hash>> &childParent, const node_ &root, const node_ &destination);

    void printAbstractPath(unique_ptr<unordered_map<node_, node_, node_::node_Hash>> &childParent, const node_ &root, const node_ &destination, Abstraction *abstraction);

    void copyAbstractNodesFromLevel(vector<AbstractNode> &abstractNodes);

    unique_ptr<RealWorld>& accessRealWorld();

    void printPathNodes(unique_ptr<unordered_map<node_, node_, node_::node_Hash>> &childParent, node_ &root, const node_ &destination);

    float computeGCost(node_ first, node_ second);

    void calculateAdjacentNodes(vector<pair<int, int>>& childNodes, node_& nextNode);

    struct InitialState {
        int startX;
        int startY;
        int goalX;
        int goalY;

        void init (int sx, int sy, int gx, int gy) {
            startX = sx;
            startY = sy;
            goalX = gx;
            goalY = gy;
        }
    };

    bool findPathToDestination();

    bool findPathToDestinationDeferred(bool earlyStop);

    node_ getNextNode(node_& current);

    bool isOnPath(node_& current);

    void printTrack(node_ root);

    int getCountOfNodesToDestination();

    void changeSourceAndDestination(int startX, int startY, int endX, int endY);

    void changeMap(vector<vector<int>> &grid);

    int compareNodeOrders(const node_ &first, const node_ &second);

    int getNodeOrder(const node_ &n);

    bool isInitialized();

    int getMaxMemoryUsed();
};


#endif //EXAMPLE_PRASTAR_H
