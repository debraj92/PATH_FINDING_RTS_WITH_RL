//
// Created by Debraj Ray on 2023-04-21.
//

#ifndef EXAMPLE_ABSTRACTNODE_H
#define EXAMPLE_ABSTRACTNODE_H

#include <iostream>
#include <unordered_set>
#include <utility>

using namespace std;

class AbstractNode {

public:

    /**
     * Color of this node from child node's perspective.
     * It decides the color and paints the child nodes.
     */
    int color;
    /**
     * Color of this node from parent node's perspective.
     * Parent node sets the color
     */
    int abstractionColor = 0;
    pair<int, int> centroidRealNode;

    /**
     * This is not an actual location. Its just the average of the nodes used for representing an
     * abstract region. Should be used to calculate G and H costs in abstract search.
     */
    pair<float, float> representationCenter;
    int totalNodesInRepresentation;

    AbstractNode(){
    }

    AbstractNode(int color_, pair<int, int> centroidReal, pair<float, float> centerRepresentation, int totalNodes) :
            color(color_), centroidRealNode(std::move(centroidReal)), representationCenter(std::move(centerRepresentation)), totalNodesInRepresentation(totalNodes) {
    }

    // have a direct edge
    unordered_set<int> reachableNodes{};

    void addChildAbstractNode(int childColor) {
        if (reachableNodes.find(childColor) == reachableNodes.end()) {
            // insert if not inserted already
            reachableNodes.insert(childColor);
        }
    }

    double getXTotalInRepresentation() {
        return totalNodesInRepresentation * representationCenter.first;
    }

    double getYTotalInRepresentation() {
        return totalNodesInRepresentation * representationCenter.second;
    }
};

#endif //EXAMPLE_ABSTRACTNODE_H
