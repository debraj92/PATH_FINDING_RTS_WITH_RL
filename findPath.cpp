//
// Created by Debraj Ray on 2021-12-30.
//

#include "findPath.h"
#include "AStar_.h"

using namespace std;

bool findPath::findPathToDestination() {
    logger->logDebug("findPath::findPathToDestination")->endLineDebug();
    isAbstractSearchInUse = false;
    bool pathFound = aStar.findPathToDestination();
    visited_x_onpath = -1;
    visited_y_onpath = -1;
    return pathFound;
}

void findPath::calculateNextPosition() {
    node_ current(visited_x_onpath, visited_y_onpath);
    if (isAbstractSearchInUse) {
        replenishNodesFromRealWorldAStar(visited_x_onpath, visited_y_onpath);
    }
    node_ next = aStar.getNextNode(current);
    next_x = next.x;
    next_y = next.y;
}

int findPath::getNext_x() {
    current_x = next_x;
    return next_x;
}

int findPath::getNext_y() {
    current_y = next_y;
    return next_y;
}

bool findPath::isOnTrack(int current_x, int current_y) {
    node_ current(current_x, current_y);
    if (isAbstractSearchInUse) {
        replenishNodesFromRealWorldAStar(current_x, current_y);
    }
    if (aStar.isOnPath(current)) {
        if (visited_x_onpath == -1) {
            visited_x_onpath = current_x;
            visited_y_onpath = current_y;
            knownOnTrackX = current_x;
            knownOnTrackY = current_y;
            return true;
        }
        if (compareNodeOrders(current_x, current_y, visited_x_onpath, visited_y_onpath) > 0) {
            visited_x_onpath = current_x;
            visited_y_onpath = current_y;
            knownOnTrackX = current_x;
            knownOnTrackY = current_y;
            return true;
        }
    }
    return false;
}

bool findPath::isOnTrackNoMemorizing(int current_x, int current_y) {
    node_ current(current_x, current_y);
    if (isAbstractSearchInUse) {
        replenishNodesFromRealWorldAStar(current_x, current_y);
    }
    bool ontrack = aStar.isOnPath(current);
    if (visited_x_onpath == -1) {
        if(ontrack) {
            knownOnTrackX = current_x;
            knownOnTrackY = current_y;
        }
        return ontrack;
    }
    if(ontrack and (compareNodeOrders(current_x, current_y, visited_x_onpath, visited_y_onpath) > 0)) {
        knownOnTrackX = current_x;
        knownOnTrackY = current_y;
        return true;
    }
    return false;
}

/**
 *
 *     --->y (columns)
 *     |
 *     v
 *     x (rows)
 */

/**
*    NW  N  NE
*       |
*     W- -E
*       |
*  SW  S  SE
*/
/// X and Y must be on track
int findPath::pathDirection(int x, int y) {
    node_ current(x,y);
    if (isAbstractSearchInUse) {
        replenishNodesFromRealWorldAStar(x, y);
    }
    node_ next = aStar.getNextNode(current);
    logger->logDebug("Current Position ")->logDebug(x)->logDebug(", ")->logDebug(y)->endLineDebug();
    logger->logDebug("Next Position ")->logDebug(next.x)->logDebug(", ")->logDebug(next.y)->endLineDebug();
    return inferDirection(x, y, next.x, next.y);
}

void findPath::changeSourceAndDestination(int startX, int startY, int endX, int endY) {
    current_x = startX;
    current_y = startY;
    source_x = startX;
    source_y = startY;
    destination_x = endX;
    destination_y = endY;
    aStar.changeSourceAndDestination(startX, startY, endX, endY);
}

void findPath::changeMap(vector<vector<int>> &grid) {
    aStar.changeMap(grid);
}

/// Warning if abstract search in use, real world A* must have been replenished
int findPath::compareNodeOrders(int location1_x, int location1_y, int location2_x, int location2_y) {
    node_ first(location1_x, location1_y);
    node_ second(location2_x, location2_y);
    return aStar.compareNodeOrders(first, second);
}

void findPath::getNextPositionAfterGivenLocation(int &given_x, int &given_y, int &next_x, int &next_y) {
    logger->logDebug("findPath::getNextPositionAfterGivenLocation")->endLineDebug();
    node_ current(given_x, given_y);
    if (isAbstractSearchInUse) {
        replenishNodesFromRealWorldAStar(given_x, given_y);
    }
    node_ next = aStar.getNextNode(current);
    next_x = next.x;
    next_y = next.y;
}

/// Warning if abstract search in use, real world A* must have been replenished
int findPath::getNodeOrder(int &x, int &y) {
    node_ givenLocation(x, y);
    return aStar.getNodeOrder(givenLocation);
}

int findPath::inferDirection(int x, int y, int next_x, int next_y) {
    logger->logDebug("findPath::inferDirection")->endLineDebug();
    if (x == next_x and y == next_y) {
        return 0;
    }
    int x_plus = x + 1;
    int x_minus = x - 1;
    int y_plus = y + 1;
    int y_minus = y - 1;
    if (x_plus == next_x && y_plus == next_y) {
        return SE;
    } else if (x == next_x && y_plus == next_y) {
        return E;
    } else if (x_minus == next_x && y_plus == next_y) {
        return NE;
    } else if (x_minus == next_x && y == next_y) {
        return N;
    } else if (x_minus == next_x && y_minus == next_y) {
        return NW;
    } else if (x == next_x && y_minus == next_y) {
        return W;
    } else if (x_plus == next_x && y_minus == next_y) {
        return SW;
    } else if (x_plus == next_x && y == next_y) {
        return S;
    }
    logger->logInfo("ERROR : Invalid pathDirection")->endLineInfo();
    return -1;
}

void findPath::createAbstractWorldGrid() {
    int gridSz = GRID_SPAN / ABSTRACT_SECTOR_SIZE;
    // initialize an empty grid
    for (int i=0; i<gridSz; i++) {
        std::vector<int> row(gridSz, 0);
        gridAbstract.push_back(row);
    }
    aStarAbs = std::make_unique<AStar_>(gridAbstract);
}

node_ findPath::getAbstractCenterOfCoordinate(int x, int y) {
    return {x / ABSTRACT_SECTOR_SIZE, y / ABSTRACT_SECTOR_SIZE};
}


bool findPath::findPathToDestinationDeferred() {
    logger->logDebug("findPath::findPathToDestinationDeferred")->endLineDebug();
    // first time isAbstractSearchInUse is false. 2nd time it's true. Baseline units will always call findPathToDestinationDeferred
    // and enemies will always call findPathToDestination
    bool isReroute = isAbstractSearchInUse;
    isAbstractSearchInUse = true;
    bool pathFound;
    node_ absCenter;

    if (not isReroute) {
        // step 1: find abstract center
        absCenter = getAbstractCenterOfCoordinate(current_x, current_y);
    } else {
        // step 1: next abstract center
        absCenter = nextAbstractCenterSaved;
    }
    // if next abstract center is abstract destination then route directly to destination
    if(absCenter == destAbsCenter) {
        aStar.changeSourceAndDestination(current_x, current_y, destination_x, destination_y);
        if (not aStar.findPathToDestination()) {
            logger->logDebug("ERROR: Path to destination NOT FOUND")->endLineDebug();
            return false;
        }
        return true;
    }

    if((not aStarAbs->isInitialized()) or (absCenter.x != aStarAbs->source.first or absCenter.y != aStarAbs->source.second)) {
        // step 2: find path to destination abstract center in abstract world
        aStarAbs->changeSourceAndDestination(absCenter.x, absCenter.y, destAbsCenter.x, destAbsCenter.y);
        pathFound = aStarAbs->findPathToDestination();
        if (not pathFound) {
            logger->logDebug("ERROR: Path to abstract center of destination NOT FOUND")->endLineDebug();
            return pathFound;
        }
    }

    // step 3: find next abstract center coordinates in real world
    int nextAbsCenterRealX, nextAbsCenterRealY;
    node_ nextAbsCenter = aStarAbs->getNextNode(absCenter);
    if (not isReroute) {
        nextAbsCenterRealX = nextAbsCenter.x * ABSTRACT_SECTOR_SIZE + ABSTRACT_SECTOR_SIZE / 2;
        nextAbsCenterRealY = nextAbsCenter.y * ABSTRACT_SECTOR_SIZE + ABSTRACT_SECTOR_SIZE / 2;
    } else {
        nextAbsCenterRealX = nextAbstractCenterSaved.x * ABSTRACT_SECTOR_SIZE + ABSTRACT_SECTOR_SIZE / 2;
        nextAbsCenterRealY = nextAbstractCenterSaved.y * ABSTRACT_SECTOR_SIZE + ABSTRACT_SECTOR_SIZE / 2;
    }
    nextAbstractCenterSaved = nextAbsCenter;

    // step 4: find path in real world to next abstract center
    aStar.changeSourceAndDestination(current_x, current_y, nextAbsCenterRealX, nextAbsCenterRealY);
    aStar.blockedDestinationCoordinateAllowed();
    pathFound = aStar.findPathToDestination();
    if (not pathFound) {
        logger->logDebug("ERROR: Could not find path to next abstract center")->endLineDebug();
        return pathFound;
    }

    visited_x_onpath = -1;
    visited_y_onpath = -1;
    return pathFound;
}


void findPath::replenishNodesFromRealWorldAStar(int x, int y) {
    logger->logDebug("replenishNodesFromRealWorldAStar")->endLineDebug();
    if(aStarAbs->childParent.empty()) return;
    node_ absCenterCurrent = getAbstractCenterOfCoordinate(x, y);
    if (absCenterCurrent == nextAbstractCenterSaved) {
        if(absCenterCurrent != destAbsCenter) {
            // reload is required from abstract world
            node_ nextAbsCenter = aStarAbs->getNextNode(absCenterCurrent);
            int nextAbsCenterRealX = nextAbsCenter.x * ABSTRACT_SECTOR_SIZE + ABSTRACT_SECTOR_SIZE / 2;
            int nextAbsCenterRealY = nextAbsCenter.y * ABSTRACT_SECTOR_SIZE + ABSTRACT_SECTOR_SIZE / 2;
            aStar.changeSourceAndDestination(current_x, current_y, nextAbsCenterRealX, nextAbsCenterRealY);
            aStar.blockedDestinationCoordinateAllowed();
            if (not aStar.findPathToDestination()) {
                logger->logDebug("ERROR: Could not find path to next abstract center")->endLineDebug();
                return;
            }
            nextAbstractCenterSaved = nextAbsCenter;
        } else {
            aStar.changeSourceAndDestination(current_x, current_y, destination_x, destination_y);
            if (not aStar.findPathToDestination()) {
                logger->logDebug("ERROR: Path to destination NOT FOUND")->endLineDebug();
            }
            nextAbstractCenterSaved = node_(-1, -1);
        }
        visited_x_onpath = -1;
        visited_y_onpath = -1;
    }
}

void findPath::stitchNewPathIntoExistingAtNode(findPath &fp_, int xOnTrack, int yOnTrack, int newSourceX, int newSourceY) {
    logger->logDebug("findPath::stitchNewPathIntoExistingAtNode")->endLineDebug();

    node_ intersectionOnTrack(xOnTrack, yOnTrack);
    // find the true intersection point (avoid a loop)
    if((newSourceX != xOnTrack or newSourceY != yOnTrack) and (xOnTrack != destination_x or yOnTrack != destination_y)) {
        node_ tempIntersectionOnTrack = intersectionOnTrack;
        node_ root(newSourceX, newSourceY);
        node_ current = fp_.aStar.childParent.find(root)->first;
        int maxNodeOrder = -1;

        while(current != intersectionOnTrack) {
            auto current_iterator = aStar.childParent.find(current);
            if (current_iterator != aStar.childParent.end() and current_iterator->first.order > maxNodeOrder) {
                maxNodeOrder = current_iterator->first.order;
                tempIntersectionOnTrack = current;
            }
            current = fp_.aStar.getNextNode(current);
        }
        if(aStar.childParent.find(intersectionOnTrack)->first.order < maxNodeOrder) {
            intersectionOnTrack = tempIntersectionOnTrack;
        }
    }

    // cut previous branch at intersection
    if(xOnTrack == destination_x and yOnTrack == destination_y) {
        aStar.childParent.clear();
    } else if (xOnTrack != source_x or yOnTrack != source_y) {
        node_ root(source_x, source_y);
        node_ current = aStar.childParent.find(root)->first;
        node_ next = aStar.getNextNode(current);
        while(next != intersectionOnTrack) {
            aStar.childParent.erase(current);
            current = next;
            next = aStar.getNextNode(current);
        }
        aStar.childParent.erase(current);
    }

    // stitch new branch
    node_ root(newSourceX, newSourceY);
    node_ current = fp_.aStar.childParent.find(root)->first;
    node_ next = fp_.aStar.getNextNode(current);
    while(current != intersectionOnTrack) {
        aStar.childParent.insert(make_pair(current, next));
        current = next;
        if(next != intersectionOnTrack) next = fp_.aStar.getNextNode(current);
    }
    source_x = newSourceX;
    source_y = newSourceY;

    visited_x_onpath = -1;
    visited_y_onpath = -1;

    // update node orders
    aStar.orderNodeLinks(node_(newSourceX, newSourceY));

}

void findPath::printTrack(int startX, int startY) {
    node_ src(startX, startY);
    aStar.printTrack(src);
}

void findPath::getCurrentStartOfPath(int &x, int &y) {
    x = aStar.source.first;
    y = aStar.source.second;
}

int findPath::getShortestDistance(int x1, int y1, int x2, int y2) {
    return aStar.findShortestDistance(make_pair(x1, y1), make_pair(x2, y2));
}

int findPath::getCountOfNodesToDestination() {
    return aStar.getCountOfNodesToDestination();
}

int findPath::getMaxMemoryUsed() {
    if (isAbstractSearchInUse) {
        return aStar.getMaxMemoryUsed() + aStarAbs->getMaxMemoryUsed();
    }
    return aStar.getMaxMemoryUsed();
}
