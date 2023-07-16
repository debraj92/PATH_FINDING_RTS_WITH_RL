//
// Created by Debraj Ray on 2023-04-21.
//

#include "RealWorld.h"
#include <cmath>
#include "gameConstants.h"

void RealWorld::printMap() {
    for(int i=0; i<GRID_SPAN; ++i) {
        for(int j=0; j<GRID_SPAN; ++j) {
            cout<<realMap[i][j];
        }
        cout<<endl;
    }
}

void RealWorld::printColors() {
    for(int i=0; i<GRID_SPAN; ++i) {
        for(int j=0; j<GRID_SPAN; ++j) {
            if(mapColors[i][j] < 0) {
                cout<<".   ";
            } else {
                cout<<mapColors[i][j];
                if (mapColors[i][j] < 10) {
                    cout<<"   ";
                } else if (mapColors[i][j] < 100) {
                    cout<<"  ";
                } else if (mapColors[i][j] >= 100) {
                    cout<<" ";
                }
            }
        }
        cout<<endl;
    }
}

void RealWorld::printProvidedRealMap(vector<vector<int>> &givenMap) {
    for(int i=0; i<GRID_SPAN; ++i) {
        for(int j=0; j<GRID_SPAN; ++j) {
            //if(givenMap[i][j] < 0) givenMap[i][j] *= -1;
            cout<<givenMap[i][j];
        }
        cout<<endl;
    }
}

vector<vector<int>> &RealWorld::getMapColors() {
    return mapColors;
}

vector<vector<int>> &RealWorld::getRealMap() {
    return realMap;
}

/**
 * Assumes x and y < 1000
 * In this case x and y < 512
 */
int RealWorld::getRank(int x, int y) {
    return x * 1000 + y;
}

float RealWorld::heuristic(int currentX, int currentY) {
    return sqrtf(powf(currentX - finalDestinationX, 2) + powf(currentY - finalDestinationY, 2));
}

void RealWorld::setGoalState(int x, int y) {
    goalX = x;
    goalY = y;
}

node_ RealWorld::createNode(int x, int y) {
    return {x, y};
}

bool RealWorld::isGoalReached(int x, int y) {
    return x == goalX && y == goalY;
}

void RealWorld::unrank(int rank, int &x, int &y) {
    y = (int)(rank % 1000);
    x = (int)(rank / 1000);
}

void RealWorld::getGoal(int &x, int &y) {
    x = goalX;
    y = goalY;
}

void RealWorld::setStart(int x, int y) {
    startX = x;
    startY = y;
}

void RealWorld::getStart(int &x, int &y) const {
    x = startX;
    y = startY;
}

void RealWorld::setPathLength(int length) {
    pathLength = length;
}

int RealWorld::getPathLength() const {
    return pathLength;
}

void RealWorld::setDestinationForHeuristics() {
    finalDestinationX = goalX;
    finalDestinationY = goalY;
}

void RealWorld::loadMap(vector<vector<int>> &grid) {
    bool resetMapColors = false;
    if(realMap.capacity() == 0) {
        realMap.resize(GRID_SPAN);
        mapColors.resize(GRID_SPAN);
        resetMapColors = true;
    }
    for(int r=0; r<GRID_SPAN; ++r) {
        if(realMap[r].capacity() == 0) {
            realMap[r].resize(GRID_SPAN);
            mapColors[r].resize(GRID_SPAN);
            resetMapColors = true;
        }
        for(int c=0; c<GRID_SPAN; ++c) {
            realMap[r][c] = grid[r][c];
            if (resetMapColors) mapColors[r][c] = -1;
        }
    }
}
