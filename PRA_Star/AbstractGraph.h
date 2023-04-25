//
// Created by Debraj Ray on 2023-04-21.
//

#ifndef EXAMPLE_ABSTRACTGRAPH_H
#define EXAMPLE_ABSTRACTGRAPH_H

#include <vector>
#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include "AbstractNode.h"
#include "RealWorld.h"
#include "Abstraction.h"

using namespace std;

class AbstractGraph : public Abstraction {

    const int SECTOR_SIZE = 16;
    unordered_map<int, AbstractNode> colorAbstractNodeMap;

    RealWorld &rworld;

    int goalColor;

    void dfs(int x, int y, int sectorBoundaryX, int sectorBoundaryY,  int color);

    int dfsInASector(int sectorStartX, int sectorStartY, int startColor);

    void createUndirectedEdge(int color1, int color2);

    double findShortestDistanceToSectorCenter(int sectorBoundaryX, int sectorBoundaryY, int x, int y);

    void dfsToConnectAbstractNodes(int x, int y, const int parentColor, vector<vector<bool>> &visited);

    void createAbstractGraphNodes();

    void connectAbstractNodesWithUndirectedEdges();

    int nodesMarked;
    int sumX, sumY;
    /**
     * Helper Coordinates to find centroid
     */
    double minDistanceCentroid;
    pair<int, int> centroid;

public:

    AbstractGraph(RealWorld &realWorld) : rworld(realWorld) {

    }

    void createAbstractGraph();

    void printNode(int color);

    void setGoalColor(int color);

    node_ createNode(int color);

    bool isGoalReached(int color);

    AbstractNode& unrank(int rank);

    int getGoalColor();

    vector<AbstractNode> getAllAbstractNodes();

    unordered_map<int, AbstractNode>& accessAbstractGraph();

    int getStartColor();

    void printConnectedColors();
};


#endif //EXAMPLE_ABSTRACTGRAPH_H