//
// Created by Debraj Ray on 2022-02-13.
//

#ifndef EXAMPLE_ASTAROPENLIST_H
#define EXAMPLE_ASTAROPENLIST_H

#include <vector>
#include "node_.h"
#include <unordered_set>

using namespace std;

class AStarOpenList {
    vector<node_> fscoreHeap;
    unordered_set<node_, node_::node_Hash> openList;
    int size = 0;
    int maxSize = 0;

    float leftChild(int parentIndex);
    float rightChild(int parentIndex);

    void sink(int k);
    void swim(int k);

public:
    void insert(node_ fvalue);
    void exchange(int idx1, int idx2);

    node_ removeMinimum();
    float peekMinimum();

    bool isEmpty();

    bool isPresent(node_& n);

    bool updateIfBetterPath(node_& n, float gvalue);

    int getMaxSize();
};


#endif //EXAMPLE_ASTAROPENLIST_H
