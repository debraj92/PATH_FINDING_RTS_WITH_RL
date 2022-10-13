//
// Created by Debraj Ray on 2022-02-13.
//

#ifndef EXAMPLE_NODE__H
#define EXAMPLE_NODE__H

#include <vector>

using namespace std;

class node_ {

public:
    int x, y;
    float f;
    float g;
    float h;
    float r;

    int heap_idx;

    int order = 0;

    struct node_Hash
    {
        size_t operator()(const node_& point) const
        {
            size_t xHash = std::hash<int>()(point.x);
            size_t yHash = std::hash<int>()(point.y) << 1;
            return xHash ^ yHash;
        }
    };

    bool operator==(const node_& otherPoint) const
    {
        if (this->x == otherPoint.x && this->y == otherPoint.y) return true;
        return false;
    }

    bool operator!=(const node_& otherPoint) const
    {
        if (this->x != otherPoint.x || this->y != otherPoint.y) return true;
        return false;
    }

    node_(int x, int y) : x(x), y(y), heap_idx(-1) {}
    node_(){}

    void computeF(float g=0, float h=0, float r=0);

    float getFScore();
};


#endif //EXAMPLE_NODE__H
