//
// Created by Debraj Ray on 2023-04-21.
//

#include "AbstractGraph.h"
#include "gameConstants.h"
#include <fstream>


void AbstractGraph::createAbstractGraphNodes() {
    int color = 1;
    int totalSectors = GRID_SPAN / SECTOR_SIZE;
    for(int sectorStartX = 0; sectorStartX < totalSectors; ++sectorStartX) {
        for(int sectorStartY = 0; sectorStartY < totalSectors; ++sectorStartY) {
            /**
             * Visit all real nodes in a sector and generate abstract nodes
             */
            color = dfsInASector(SECTOR_SIZE * sectorStartX, SECTOR_SIZE * sectorStartY, color);
        }
    }
}

void AbstractGraph::dfs(int x, int y, int sectorBoundaryX, int sectorBoundaryY, int color) {
    /**
     * Check x, y lies within sector
     */
    if (x >= sectorBoundaryX || y >= sectorBoundaryY) {
        return;
    }
    if (x < sectorBoundaryX - SECTOR_SIZE || y < sectorBoundaryY - SECTOR_SIZE) {
        return;
    }

    if (rworld.getRealMap()[x][y] < 0) {
        // obstacle
        return;
    }

    if (rworld.getMapColors()[x][y] == -1) {
        // unvisited -> visit it
        rworld.getMapColors()[x][y] = color;
        /**
         * Mark centroid of this abstract Node based on distance to the center of the sector.
         */
        findShortestDistanceToSectorCenter(sectorBoundaryX, sectorBoundaryY, x, y);
        sumX += x;
        sumY += y;
        ++nodesMarked;
    } else {
        // already visited
        return;
    }

    /**
     * 4 possible directions to visit: up, down, left, right
     */
    if (x > 0) {
        dfs(x - 1, y, sectorBoundaryX, sectorBoundaryY, color);
    }
    if (y > 0) {
        dfs(x, y - 1, sectorBoundaryX, sectorBoundaryY, color);
    }
    if (x < GRID_SPAN - 1) {
        dfs(x + 1, y, sectorBoundaryX, sectorBoundaryY, color);
    }
    if (y < GRID_SPAN - 1) {
        dfs(x, y + 1, sectorBoundaryX, sectorBoundaryY, color);
    }

    /// diagonal moves
    if (x > 0 && y > 0) {
        dfs(x - 1, y - 1, sectorBoundaryX, sectorBoundaryY, color);
    }
    if (x > 0 && y < GRID_SPAN - 1) {
        dfs(x - 1, y + 1, sectorBoundaryX, sectorBoundaryY, color);
    }
    if (x < GRID_SPAN - 1 && y > 0) {
        dfs(x + 1, y - 1, sectorBoundaryX, sectorBoundaryY, color);
    }
    if (x < GRID_SPAN - 1 && y < GRID_SPAN - 1) {
        dfs(x + 1, y + 1, sectorBoundaryX, sectorBoundaryY, color);
    }
}

int AbstractGraph::dfsInASector(int sectorStartX, int sectorStartY,int startColor) {
    for(int x = sectorStartX; x < sectorStartX + SECTOR_SIZE; ++x) {
        for(int y = sectorStartY; y < sectorStartY + SECTOR_SIZE; ++y) {
            if (rworld.getMapColors()[x][y] == -1 && rworld.getRealMap()[x][y] >= 0) {
                // This location is NOT an obstacle and is not already coloured
                /**
                 * find connected component starting from this node that lies inside the sector.
                 * Record the centroid, which is the point closest to the sector midpoint
                 */
                minDistanceCentroid = 1000;
                nodesMarked = 0;
                sumX = 0;
                sumY = 0;
                dfs(x, y, sectorStartX + SECTOR_SIZE, sectorStartY + SECTOR_SIZE, startColor);
                if (nodesMarked > 0) {
                    pair<double, double> representation = {sumX / nodesMarked, sumY / nodesMarked};
                    colorAbstractNodeMap.insert({startColor, {startColor, centroid, representation, nodesMarked}});
                    ++startColor;
                }
            }
        }
    }
    return startColor;
}

void AbstractGraph::createUndirectedEdge(int color1, int color2) {
    colorAbstractNodeMap.find(color1)->second.addChildAbstractNode(color2);
    colorAbstractNodeMap.find(color2)->second.addChildAbstractNode(color1);
}

void AbstractGraph::connectAbstractNodesWithUndirectedEdges() {
    for (const auto& color_AbsNode: colorAbstractNodeMap) {
        /**
         * Create an empty 2d visited array for DFS
         */
        vector<vector<bool>> visited(SECTOR_SIZE);
        for(int i=0; i<SECTOR_SIZE; ++i) {
            visited[i].reserve(SECTOR_SIZE);
            for(int j=0; j<SECTOR_SIZE; ++j) {
                visited[i][j] = false;
            }
        }
        const auto& absNode = color_AbsNode.second;
        dfsToConnectAbstractNodes(absNode.centroidRealNode.first, absNode.centroidRealNode.second, absNode.color, visited);
        //printNode(absNode.color);
    }
}

void AbstractGraph::dfsToConnectAbstractNodes(int x, int y, const int parentColor, vector<vector<bool>> &visited) {
    if (x >= GRID_SPAN || y >= GRID_SPAN) {
        return;
    }
    if (x < 0 || y < 0) {
        return;
    }
    if(visited[x % SECTOR_SIZE][y % SECTOR_SIZE]) {
        return;
    }
    visited[x % SECTOR_SIZE][y % SECTOR_SIZE] = true;
    /**
     * 4 possible directions to visit: up, down, left, right
     * Create edge between abstract nodes if two neighbouring real nodes have different colours.
     * A colour of -1 implies a node not passable and therefore not interesting
     */
    if (x > 0 && rworld.getMapColors()[x-1][y] != -1) {
        if(rworld.getMapColors()[x-1][y] == parentColor) {
            dfsToConnectAbstractNodes(x-1, y, parentColor, visited);
        } else {
            createUndirectedEdge(parentColor, rworld.getMapColors()[x-1][y]);
        }
    }
    if (y > 0 && rworld.getMapColors()[x][y-1] != -1) {
        if(rworld.getMapColors()[x][y-1] == parentColor) {
            dfsToConnectAbstractNodes(x, y-1, parentColor, visited);
        } else {
            createUndirectedEdge(parentColor, rworld.getMapColors()[x][y-1]);
        }
    }
    if (x < GRID_SPAN - 1 && rworld.getMapColors()[x+1][y] != -1) {
        if(rworld.getMapColors()[x+1][y] == parentColor) {
            dfsToConnectAbstractNodes(x+1, y, parentColor, visited);
        } else {
            createUndirectedEdge(parentColor, rworld.getMapColors()[x+1][y]);
        }
    }
    if (y < GRID_SPAN - 1 && rworld.getMapColors()[x][y+1] != -1) {
        if(rworld.getMapColors()[x][y+1] == parentColor) {
            dfsToConnectAbstractNodes(x, y+1, parentColor, visited);
        } else {
            createUndirectedEdge(parentColor, rworld.getMapColors()[x][y+1]);
        }
    }

    /// Diagonal Moves
    if (x > 0 && y > 0 && rworld.getMapColors()[x-1][y-1] != -1) {
        if(rworld.getMapColors()[x-1][y-1] == parentColor) {
            dfsToConnectAbstractNodes(x-1, y-1, parentColor, visited);
        } else {
            createUndirectedEdge(parentColor, rworld.getMapColors()[x-1][y-1]);
        }
    }

    if (x > 0 && y < GRID_SPAN - 1 && rworld.getMapColors()[x-1][y+1] != -1) {
        if(rworld.getMapColors()[x-1][y+1] == parentColor) {
            dfsToConnectAbstractNodes(x-1, y+1, parentColor, visited);
        } else {
            createUndirectedEdge(parentColor, rworld.getMapColors()[x-1][y+1]);
        }
    }

    if (x < GRID_SPAN - 1 && y > 0 && rworld.getMapColors()[x+1][y-1] != -1) {
        if(rworld.getMapColors()[x+1][y-1] == parentColor) {
            dfsToConnectAbstractNodes(x+1, y-1, parentColor, visited);
        } else {
            createUndirectedEdge(parentColor, rworld.getMapColors()[x+1][y-1]);
        }
    }

    if (x < GRID_SPAN - 1 && y < GRID_SPAN - 1 && rworld.getMapColors()[x+1][y+1] != -1) {
        if(rworld.getMapColors()[x+1][y+1] == parentColor) {
            dfsToConnectAbstractNodes(x+1, y+1, parentColor, visited);
        } else {
            createUndirectedEdge(parentColor, rworld.getMapColors()[x+1][y+1]);
        }
    }
}

void AbstractGraph::createAbstractGraph() {
    createAbstractGraphNodes();
    connectAbstractNodesWithUndirectedEdges();
    //printConnectedColors();
}

void AbstractGraph::printNode(int color) {

    const auto &color_AbsNode = colorAbstractNodeMap.find(color);
    cout<<"Color: "<<color_AbsNode->first<<", Connected Colors: ";
    for(const auto& childColor: color_AbsNode->second.reachableNodes) {
        cout<<childColor<<"  ";
    }
    cout<<endl;
}

void AbstractGraph::setGoalColor(int color) {
    goalColor = color;
}

node_ AbstractGraph::createNode(int color) {
    return {color, 1};
}

bool AbstractGraph::isGoalReached(int color) {
    return goalColor == color;
}

AbstractNode& AbstractGraph::unrank(int rank) {

    assert(colorAbstractNodeMap.find(rank) != colorAbstractNodeMap.end());
    return colorAbstractNodeMap.find(rank)->second;
}

int AbstractGraph::getGoalColor() {
    return goalColor;
}


double AbstractGraph::findShortestDistanceToSectorCenter(int sectorBoundaryX, int sectorBoundaryY, int x, int y) {
    int sectorMidX = sectorBoundaryX - (SECTOR_SIZE / 2);
    int sectorMidY = sectorBoundaryY - (SECTOR_SIZE / 2);
    double currentMin = sqrt(pow(x - sectorMidX, 2) + pow(y - sectorMidY, 2));
    if (currentMin < minDistanceCentroid) {
        minDistanceCentroid = currentMin;
        centroid = {x, y};
    }
    return currentMin;
}

vector<AbstractNode> AbstractGraph::getAllAbstractNodes() {
    vector<AbstractNode> allNodes;
    for (auto& iteratorColorNode: colorAbstractNodeMap) {
        allNodes.emplace_back(iteratorColorNode.second);
    }
    return move(allNodes);
}

/**
 * Real World must contain the actual start (x,y)
 */
int AbstractGraph::getStartColor() {
    int startX, startY;
    rworld.getStart(startX, startY);
    int startColor = rworld.getMapColors()[startX][startY];
    return startColor;
}

unordered_map<int, AbstractNode>& AbstractGraph::accessAbstractGraph() {
    return colorAbstractNodeMap;
}

void AbstractGraph::printConnectedColors() {
    ofstream myfile;
    myfile.open ("/Users/debrajray/MyComputer/RL-A-STAR-THESIS/colors.csv");
    for(int i=0; i<GRID_SPAN; ++i) {
        for(int j=0; j<GRID_SPAN; ++j) {
            int abGColor = 0;
            if (rworld.getRealMap()[i][j] == 0) {
                abGColor = rworld.getMapColors()[i][j];
            }
            if(!abGColor) {
                myfile<<".   ";
            } else {
                myfile<<abGColor;
                if (abGColor < 10) {
                    myfile<<"   ";
                } else if (abGColor < 100) {
                    myfile<<"  ";
                } else if (abGColor >= 100) {
                    myfile<<" ";
                }
            }
        }
        myfile<<endl;
    }
    myfile.close();
}