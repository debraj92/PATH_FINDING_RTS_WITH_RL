//
// Created by Debraj Ray on 2023-04-19.
//

#include "PRAStar.h"

void
PRAStar::insertIntoOpenList(AStarOpenList &openList, unique_ptr<unordered_set<node_, node_::node_Hash>> &closedList,
                            unique_ptr<unordered_map<node_, node_, node_::node_Hash>> &childParent, node_ childNode,
                            node_ &parentNode, const function<float(node_)> &hCost, float gCost) {

    // do not insert in open list if node already exists in closed
    if (closedList->find(childNode) != closedList->end()) {
        return;
    }
    if (!openList.isPresent(childNode)) {
        /**
         * New node, never visited. Just insert in open.
         */
        childNode.computeF(parentNode.g + gCost, hCost(childNode));
        openList.insert(childNode);
        // child -> parent
        if (childParent->find(childNode) != childParent->end()) {
            childParent->find(childNode)->second = parentNode;
        } else {
            childParent->insert({childNode, parentNode});
        }
    } else {
        /**
         * Node exists in open. Insert if better path found
         */
        if (openList.updateIfBetterPath(childNode, parentNode.g + gCost)) {
            childParent->find(childNode)->second = parentNode;
        }
    }
}

void PRAStar::finalizeNodeLinks(unique_ptr<unordered_map<node_, node_, node_::node_Hash>> &childParent,
                                unique_ptr<unordered_set<node_, node_::node_Hash>> &closedList) {

    auto childParentIterator = childParent->begin();
    while(childParentIterator != childParent->end()) {
        auto child = childParentIterator->first;
        auto parent = childParentIterator->second;
        if (closedList->find(child) == closedList->end() || closedList->find(parent) == closedList->end()) {
            childParentIterator++;
            childParent->erase(child);
        } else {
            childParentIterator++;
        }
    }
}

int
PRAStar::reverseNodeLinks(node_ current, unique_ptr<unordered_map<node_, node_, node_::node_Hash>> &childParent) {
    auto parent = childParent->find(current)->second;
    auto grandParent = childParent->find(parent)->second;
    int count = 0;
    while (parent != grandParent) {
        (*childParent)[parent] = current;
        current = parent;
        parent = grandParent;
        grandParent = childParent->find(parent)->second;
        count++;
    }
    (*childParent)[parent] = current;
    return count+1;
}

void PRAStar::createAbstractGraph() {
    abstractGraph->createAbstractGraph();
}

void PRAStar::createRealWorld(vector<vector<int>> &grid) {
    realWorld = make_unique<RealWorld>();
    realWorld->loadMap(grid);
}

void PRAStar::copyRealWorld(vector<vector<int>> &copied) {
    for(int row = 0; row < GRID_SPAN; ++row) {
        copied[row].reserve(GRID_SPAN);
        for(int col = 0; col < GRID_SPAN; ++col) {
            copied[row][col] = realWorld->getRealMap()[row][col];
        }
    }
}

void PRAStar::initStartState(int startX, int startY) {
    realWorld->setStart(startX, startY);
}

void PRAStar::initGoalState(int goalX, int goalY) {
    realWorld->setGoalState(goalX, goalY);
    realWorld->setDestinationForHeuristics();
    abstractGraph->setGoal(goalX, goalY);
    abstractGraph->setGoalColor(realWorld->getMapColors()[goalX][goalY]);
}

bool PRAStar::searchPathInRealWorldWithAstar(const int parentGoal) {
    /**
     * Generalizes the heuristic function with a common interface
     */
    auto heuristicFunc = [this](node_ node) {
        return realWorld->heuristic(node.x, node.y);
    };
    pathRealWorld->clear();
    auto closedList = make_unique<unordered_set<node_, node_::node_Hash>>();
    AStarOpenList openList;
    int startX, startY;
    realWorld->getStart(startX, startY);
    /// Starting point should be passable coordinate. Or non-obstacle.
    if (realWorld->getRealMap()[startX][startY] < 0) {
        logger->logInfo("Starting point should be passable (non-obstacle) coordinate.")->endLineInfo();
        return false;
    }
    auto root = realWorld->createNode(startX, startY);
    auto h = realWorld->heuristic(startX, startY);
    root.computeF(0, h);
    openList.insert(root);
    pathRealWorld->insert({root, root});
    int x, y;
    bool isPathFound = false;
    int goalX, goalY;
    /**
     * Moving Goal
     */
    realWorld->getGoal(goalX, goalY);
    while(!openList.isEmpty()) {
        auto nextNode = openList.removeMinimum();
        closedList->insert(nextNode);

        // un-rank
        x = nextNode.x;
        y = nextNode.y;

        /**
         * Additional termination condition for PRA*:
         * If abstract target node is destination node
         *  - RealWorld search should go till actual destination x, y
         *  - This case is similar to plain A* search
         * Else
         *  - RealWorld search should terminate at any x,y in destination node
         */

        if (realWorld->isGoalReached(x, y) || (parentGoal > 0 && realWorld->getMapColors()[x][y] == parentGoal)) {
            //finalizeNodeLinks(pathRealWorld, closedList);
            auto solutionLength = reverseNodeLinks(nextNode, pathRealWorld);
            //cout<<"Path Length Real World "<<solutionLength<<endl;
            //printRealPath(pathRealWorld, root, nextNode);
            orderNodeLinks(root, nextNode);
            eraseDestinationNode(nextNode);
            isPathFound = true;
            if (!abstractParentNodeColors->empty()) {
                /**
                 * PRA*
                 */
                /// We update start point for PRA*
                realWorld->setStart(x, y);
                // We aggregate path length for PRA*.
                realWorld->setPathLength(solutionLength + realWorld->getPathLength());
            } else {
                realWorld->setPathLength(solutionLength);
            }
            break;
        }

        // generate moves
        vector<pair<int, int>> childNodes;
        calculateAdjacentNodes(childNodes, nextNode);
        for (pair<int, int> node_pair : childNodes) {
            if (abstractParentNodeColors->empty() ||
                    abstractParentNodeColors->find(realWorld->getMapColors()[node_pair.first][node_pair.second]) != abstractParentNodeColors->end()) {

                auto childNode = realWorld->createNode(node_pair.first, node_pair.second);
                insertIntoOpenList(openList, closedList, pathRealWorld, childNode, nextNode, heuristicFunc,
                                   computeGCost(childNode, nextNode));
            }
        }
    }

    if (!isPathFound) {
        logger->logDebug("Path Not Found")->endLineDebug();
    }

    return isPathFound;
}

bool PRAStar::searchPathInAbstractGraphWithAstar(unique_ptr<unordered_map<node_, node_, node_::node_Hash>> &childParent) {
    Abstraction *abstraction = abstractGraph.get();
    auto closedList = make_unique<unordered_set<node_, node_::node_Hash>>();
    auto startColor = abstraction->getStartColor();

    if (startColor == -1) {
        logger->logDebug("Abstract Path Not Found")->endLineDebug();
        return false;
    }
    /**
     * Generalizes the heuristic function with a common interface
     */
    auto heuristicFunc = [&abstraction](node_ node) {
        return abstraction->heuristic(node.x);
    };
    AStarOpenList openList;
    auto root = abstraction->createNode(startColor);
    auto h = abstraction->heuristic(startColor);
    root.computeF(0, h);
    openList.insert(root);
    childParent->insert({root, root});
    bool isPathFound = false;
    /**
     * LastMile if pathfinding for the highest level of abstraction or the actual goal of current abstraction lies in the
     * parentGoal region
     */
    while(!openList.isEmpty()) {
        auto nextNode = openList.removeMinimum();
        closedList->insert(nextNode);

        const AbstractNode &currentNode = abstraction->unrank(nextNode.x);
        if (abstraction->isGoalReached(currentNode.color)) {
            /**
             * For last mile, terminate only if goal is reached.
             * Otherwise, terminate if abstract parent node is reached
             */
            //finalizeNodeLinks(childParent, closedList);
            auto solutionLength = reverseNodeLinks(nextNode, childParent);
            abstraction->setSolutionLength(solutionLength);
            //cout<<"Path Length Abstract World "<<solutionLength<<endl;
            isPathFound = true;
            break;
        }

        // insert child nodes into open list
        for(int childColor : currentNode.reachableNodes) {
            const auto &childNode = abstraction->createNode(childColor);
            const auto &childAbstractNode = abstraction->unrank(childColor);
            insertIntoOpenList(openList, closedList, childParent, childNode, nextNode,
                               heuristicFunc, abstraction->getGCost(childAbstractNode, currentNode));
        }
    }
    if(!isPathFound) {
        logger->logDebug("Abstract Path Not Found")->endLineDebug();
    }
    return isPathFound;
}

int PRAStar::getGoalColorOfAbstraction() {
    return abstractGraph->getGoalColor();
}

void PRAStar::printRealPath(unique_ptr<unordered_map<node_, node_, node_::node_Hash>> &childParent, const node_ &root,
                            const node_ &destination) {
    vector<vector<int>> copied(GRID_SPAN);
    copyRealWorld(copied);
    int x, y;
    auto current = childParent->find(root)->first;
    x = current.x;
    y = current.y;
    copied[x][y] = 9;
    while(current != destination) {
        current = childParent->find(current)->second;
        x = current.x;
        y = current.y;
        copied[x][y] = 9;
    }
    realWorld->printProvidedRealMap(copied);
}

void PRAStar::printAbstractPath(unique_ptr<unordered_map<node_, node_, node_::node_Hash>> &childParent, const node_ &root,
                                const node_ &destination, Abstraction *abstraction) {

    auto current = childParent->find(root)->first;
    cout<<"Abstract Path Colours "<<endl;
    cout<<current.x<<" ";
    while(current != destination) {
        current = childParent->find(current)->second;
        cout<<current.x<<" ";
    }
    cout<<endl;


    //// Prints Abstract path in real world
    vector<vector<int>> copied(GRID_SPAN);
    copyRealWorld(copied);
    current = childParent->find(root)->first;
    auto abstractNode = abstraction->unrank(current.x);
    copied[(int)round(abstractNode.representationCenter.first)][(int)round(abstractNode.representationCenter.second)] = 9;
    while(current != destination) {
        current = childParent->find(current)->second;
        abstractNode = abstraction->unrank(current.x);
        copied[(int)round(abstractNode.representationCenter.first)][(int)round(abstractNode.representationCenter.second)] = 9;
    }
    realWorld->printProvidedRealMap(copied);
}

void PRAStar::copyAbstractNodesFromLevel(vector<AbstractNode> &abstractNodes) {
    abstractNodes = abstractGraph->getAllAbstractNodes();
}

unique_ptr<RealWorld> &PRAStar::accessRealWorld() {
    return realWorld;
}

void PRAStar::printPathNodes(unique_ptr<unordered_map<node_, node_, node_::node_Hash>> &childParent, node_ &root, const node_ &destination) {

    int x, y;
    int count = 0;
    while(root != destination) {
        auto current = childParent->find(root);
        auto this_node = current->first;
        auto next_node = current->second;
        x = this_node.x;
        y = this_node.y;
        cout<<"("<<x<<","<<y<<") ";
        ++count;
        root = next_node;
    }
    cout<<endl;
    cout<<"Total Nodes in Path "<<count<<endl;
}

bool PRAStar::findPathToDestination() {
    realWorld->setPathLength(0);
    if (!searchPathInRealWorldWithAstar()) {
        return false;
    }
    return true;
}

/// requires current node to be on path
node_ PRAStar::getNextNode(node_ &current) {
    if(isDestinationFound(current)) {
        logger->logInfo("Next node after destination is destination (loop?)");
        return current;
    }
    assert(pathRealWorld->find(current) != pathRealWorld->end());
    return pathRealWorld->find(current)->second;
}

/// Only for real world
bool PRAStar::isDestinationFound(const node_ &node) {
    int destX, destY;
    realWorld->getGoal(destX, destY);
    return node.x == destX and node.y == destY;
}

bool PRAStar::isOnPath(node_ &current) {
    if(pathRealWorld->empty()) return false;
    return pathRealWorld->find(current) != pathRealWorld->end();
}

void PRAStar::printTrack(node_ root) {
    int destX, destY;
    realWorld->getGoal(destX, destY);
    auto destNode = realWorld->createNode(destX, destY);
    printPathNodes(pathRealWorld, root, destNode);
}

int PRAStar::getCountOfNodesToDestination() {
    return realWorld->getPathLength();
}

void PRAStar::changeSourceAndDestination(int startX, int startY, int endX, int endY) {
    initStartState(startX, startY);
    initGoalState(endX, endY);
    initialized = true;
}

/// Careful, it does not update abstraction
/// Can be used to place small temporary obstacles that do not affect abstractions
void PRAStar::changeMap(vector<vector<int>> &grid) {
    realWorld->loadMap(grid);
}

/// Both nodes must be on path (excludes destination)
int PRAStar::compareNodeOrders(const node_ &first, const node_ &second) {
    if (pathRealWorld->find(first) == pathRealWorld->end() or pathRealWorld->find(second) == pathRealWorld->end()) {
        logger->logInfo("Nodes to compare are absent or marked as destination")->endLineInfo();
        return -100;
    }
    return pathRealWorld->find(first)->first.order - pathRealWorld->find(second)->first.order;
}

void PRAStar::orderNodeLinks(node_ root, node_ dest) {
    int order = 1;
    while(root != dest) {
        auto current = pathRealWorld->find(root);
        auto this_node = current->first;
        auto next_node = current->second;
        this_node.order = order++;
        next_node.order = order;
        pathRealWorld->erase(this_node);
        pathRealWorld->insert({this_node, next_node});
        root = next_node;
    }
}

void PRAStar::eraseDestinationNode(node_ dest) {
    assert(pathRealWorld->find(dest) != pathRealWorld->end());
    pathRealWorld->erase(dest);
}

int PRAStar::getNodeOrder(const node_ &n) {
    assert(pathRealWorld->find(n) != pathRealWorld->end());
    return pathRealWorld->find(n)->first.order;
}

bool PRAStar::isInitialized() {
    return initialized;
}

float PRAStar::computeGCost(node_ first, node_ second) {
    int g_distance = abs(first.x - second.x) + abs(first.y - second.y);
    return g_distance == 1 ? 1 : 1.5;
}

void PRAStar::calculateAdjacentNodes(vector<pair<int, int>> &childNodes, node_ &nextNode) {
    /**
             * 8 possible moves
             * i-1, j-1
             * i-1, j
             * i-1, j+1
             * i,j-1
             * i, j+1
             * i+1,j-1
             * i+1,j
             * i+1, j+1
             */

    if(nextNode.x-1 >= 0) {
        //j-1, j, j+1 : 3 moves
        if (nextNode.y-1 >= 0) {
            addEdge(nextNode.x-1, nextNode.y-1, childNodes);
        }
        if (nextNode.y+1 < GRID_SPAN) {
            addEdge(nextNode.x-1, nextNode.y+1, childNodes);
        }
        addEdge(nextNode.x-1, nextNode.y, childNodes);
    }
    if(nextNode.x+1 < GRID_SPAN) {
        //j-1, j, j+1 : 3 moves
        if (nextNode.y-1 >= 0) {
            addEdge(nextNode.x+1, nextNode.y-1, childNodes);
        }
        if (nextNode.y+1 < GRID_SPAN) {
            addEdge(nextNode.x+1, nextNode.y+1, childNodes);
        }
        addEdge(nextNode.x+1, nextNode.y, childNodes);
    }
    if (nextNode.y-1 >= 0) {
        // i : 1 move
        addEdge(nextNode.x, nextNode.y-1, childNodes);
    }
    if (nextNode.y+1 < GRID_SPAN) {
        // i : 1 move
        addEdge(nextNode.x, nextNode.y+1, childNodes);
    }
}

void PRAStar::addEdge(int nextX, int nextY, vector<pair<int, int>> &nodes) {
    if (realWorld->getRealMap()[nextX][nextY] >= 0) {
        nodes.emplace_back(nextX, nextY);
    }
}

void PRAStar::resetAbstractPath() {
    abstractParentNodeColors->clear();
}

int PRAStar::getMaxMemoryUsed() {
    return maxSizeOfOpenList;
}

bool PRAStar::findPathToDestinationDeferred(bool earlyStop) {
    assert(initialized);

    realWorld->setPathLength(0);
    unique_ptr<unordered_map<node_, node_, node_::node_Hash>> abstractChildParent = std::make_unique<unordered_map<node_, node_, node_::node_Hash>>();
    if (!searchPathInAbstractGraphWithAstar(abstractChildParent)) {
        return false;
    }
    /// truncate for early stop. This is PRA* (4)
    bool isBaselineActive = earlyStop;
    earlyStop = earlyStop && abstractGraph->getSolutionLength() > 4;
    int currentColor = abstractGraph->getStartColor();
    int goalColor = abstractGraph->getGoalColor();
    int i= 1;
    int intermediateParentGoal = -1;
    while(currentColor != goalColor) {
        intermediateParentGoal = currentColor;
        abstractParentNodeColors->insert(currentColor);
        auto abNode = abstractGraph->createNode(currentColor);
        currentColor = abstractChildParent->find(abNode)->second.x;
        ++i;
        if(earlyStop && i>4) {
            break;
        }
    }
    if(!earlyStop) {
        abstractParentNodeColors->insert(goalColor);
        intermediateParentGoal = -1;
    }
    if (isBaselineActive) {
        abstractParentNodeColors->clear();
    }
    if (!searchPathInRealWorldWithAstar(intermediateParentGoal)) {
        return false;
    }
    return true;
}






