//
// Created by Debraj Ray on 2023-04-21.
//

#ifndef EXAMPLE_REALWORLD_H
#define EXAMPLE_REALWORLD_H

#include <vector>
#include <iostream>
#include "node_.h"

using namespace std;

class RealWorld {

    vector<vector<int>> realMap;
    vector<vector<int>> mapColors;

    int goalX, goalY;
    int finalDestinationX, finalDestinationY;

    int startX, startY;
    int pathLength = 0;

public:

    void loadMap(vector<vector<int>> &grid);

    void printMap();
    void printProvidedRealMap(vector<vector<int>> &givenMap);

    void printColors();

    vector<vector<int>>& getMapColors();
    vector<vector<int>>& getRealMap();

    int getRank(int x, int y);

    void unrank(int rank, int &x, int &y);

    void setGoalState(int x, int y);

    void setDestinationForHeuristics();

    float heuristic(int currentX, int currentY);

    node_ createNode(int x, int y);

    bool isGoalReached(int x, int y);

    void getGoal(int &x, int &y);

    void setStart(int x, int y);

    void getStart(int &x, int &y) const;

    void setPathLength(int length);

    int getPathLength() const;

};


#endif //EXAMPLE_REALWORLD_H
