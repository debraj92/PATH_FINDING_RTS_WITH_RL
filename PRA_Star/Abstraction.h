//
// Created by Debraj Ray on 2023-04-21.
//

#ifndef EXAMPLE_ABSTRACTION_H
#define EXAMPLE_ABSTRACTION_H

#include "AbstractNode.h"
#include "node_.h"

class Abstraction {

public:

    int goalX, goalY, solutionLength;

    virtual void setGoalColor(int color) = 0;

    virtual node_ createNode(int color) = 0;

    virtual bool isGoalReached(int color) = 0;

    virtual AbstractNode& unrank(int rank) = 0;

    virtual int getGoalColor() = 0;

    virtual vector<AbstractNode> getAllAbstractNodes() = 0;

    virtual int getStartColor() = 0;

    virtual inline float getGCost(const AbstractNode &n1, const AbstractNode &n2) {

        return sqrtf(powf(n1.representationCenter.first - n2.representationCenter.first, 2) +
                    powf(n1.representationCenter.second - n2.representationCenter.second, 2));
    };

    virtual unordered_map<int, AbstractNode>& accessAbstractGraph() = 0;

    virtual void createAbstractGraphNodes() = 0;

    void setGoal(int goalx, int goaly) {
        this->goalX = goalx;
        this->goalY = goaly;
    }

    virtual float heuristic(int nodeColor) {
        const auto &node = unrank(nodeColor);
        return sqrtf(powf(node.representationCenter.first - goalX, 2) +
                    powf(node.representationCenter.second - goalY, 2));

    };

    virtual inline double findShortestDistanceBetweenNodes(const AbstractNode &node1, const AbstractNode &node2) {
        return sqrt(pow(node1.representationCenter.first - node2.representationCenter.first, 2) +
                    pow(node1.representationCenter.second - node2.representationCenter.second,2));
    }

    void setSolutionLength(int solnLength) {
        this->solutionLength = solnLength;
    }

    int getSolutionLength() {
        return this->solutionLength;
    }

};

#endif //EXAMPLE_ABSTRACTION_H
