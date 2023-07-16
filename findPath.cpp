//
// Created by Debraj Ray on 2023-04-19.
//

#include "findPath.h"

int findPath::compareNodeOrders(int location1_x, int location1_y, int location2_x, int location2_y) {
    node_ first(location1_x, location1_y);
    node_ second(location2_x, location2_y);
    return aStar.compareNodeOrders(first, second);
}

bool findPath::findPathToDestination() {
    logger->logDebug("findPath::findPathToDestination")->endLineDebug();
    bool pathFound = aStar.findPathToDestination();
    visited_x_onpath = -1;
    visited_y_onpath = -1;
    return pathFound;
}

bool findPath::findPathToDestinationDeferred(bool earlyStop, bool useConstrainedAStar) {
    logger->logDebug("findPath::findPathToDestinationDeferred")->endLineDebug();
    bool pathFound = aStar.findPathToDestinationDeferred(earlyStop, useConstrainedAStar);
    visited_x_onpath = -1;
    visited_y_onpath = -1;
    return pathFound;
}

void findPath::calculateNextPosition() {
    node_ current(visited_x_onpath, visited_y_onpath);
    node_ next = aStar.getNextNode(current);
    next_x = next.x;
    next_y = next.y;
}

void findPath::getNextPositionAfterGivenLocation(int &given_x, int &given_y, int &next_x, int &next_y) {
    logger->logDebug("findPath::getNextPositionAfterGivenLocation")->endLineDebug();
    node_ current(given_x, given_y);
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

int findPath::pathDirection(int x, int y) {
    node_ current(x,y);
    node_ next = aStar.getNextNode(current);
    logger->logDebug("Current Position ")->logDebug(x)->logDebug(", ")->logDebug(y)->endLineDebug();
    logger->logDebug("Next Position ")->logDebug(next.x)->logDebug(", ")->logDebug(next.y)->endLineDebug();
    return inferDirection(x, y, next.x, next.y);
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
    throw runtime_error("ERROR : Invalid pathDirection");
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

void findPath::changeMap(std::vector<std::vector<int>> &grid) {
    aStar.changeMap(grid);
}

int findPath::getCountOfNodesToDestination() {
    return aStar.getCountOfNodesToDestination();
}

float findPath::findShortestDistance(pair<int, int> src, pair<int, int> dst) {
    return max(abs(src.first - dst.first), abs(src.second - dst.second));
}

int findPath::getShortestDistance(int x1, int y1, int x2, int y2) {
    return findShortestDistance(make_pair(x1, y1), make_pair(x2, y2));
}

void findPath::getCurrentStartOfPath(int &x, int &y) {
    aStar.accessRealWorld()->getStart(x, y);
}

int findPath::getMaxMemoryUsed() {
    return aStar.getMaxMemoryUsed();
}

int findPath::getNodeOrder(int &x, int &y) {
    node_ givenLocation(x, y);
    return aStar.getNodeOrder(givenLocation);
}

void findPath::createAbstractGraph() {
    aStar.createAbstractGraph();
}


