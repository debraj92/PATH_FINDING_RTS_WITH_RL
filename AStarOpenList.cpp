//
// Created by Debraj Ray on 2022-02-13.
//

#include <iostream>
#include "AStarOpenList.h"

float AStarOpenList::leftChild(int parentIndex) {
    return fscoreHeap[2*parentIndex + 1].getFScore();
}

float AStarOpenList::rightChild(int parentIndex) {
    return fscoreHeap[2*parentIndex + 2].getFScore();
}

void AStarOpenList::sink(int k) {
    int leftChildIdx, rightChildIdx, minChildIdx;
    while(2*k + 1 <size) {
        leftChildIdx = 2*k + 1;
        rightChildIdx = 2*k + 2;
        if (rightChildIdx < size) {
            minChildIdx = rightChildIdx;
            if (leftChild(k) < rightChild(k)) {
                minChildIdx = leftChildIdx;
            }
            if(fscoreHeap[minChildIdx].getFScore() < fscoreHeap[k].getFScore()){
                exchange(minChildIdx, k);
                k = minChildIdx;
            } else {
                // already a heap
                break;
            }
        } else {
            if (leftChild(k) < fscoreHeap[k].getFScore()) {
                exchange(leftChildIdx, k);
                k = leftChildIdx;
            } else {
                // already a heap
                break;
            }
        }
    }
}

void AStarOpenList::swim(int k) {
    int parent_index;
    while (k>=1) {
        parent_index = (k - 1) / 2;
        if (fscoreHeap[k].getFScore() < fscoreHeap[parent_index].getFScore()) {
            exchange(k, parent_index);
            k = parent_index;
        } else {
            break;
        }
    }
}

void AStarOpenList::insert(node_ node) {
    node.heap_idx = size;
    fscoreHeap.push_back(node);
    openList.insert(node);
    size++;
    maxSize = size > maxSize ? size : maxSize;
    swim(size-1);
}

void AStarOpenList::exchange(int idx1, int idx2) {
    if (idx1 == idx2) {
        return;
    }
    node_ temp = fscoreHeap[idx1];
    fscoreHeap[idx1] = fscoreHeap[idx2];
    fscoreHeap[idx2] = temp;

    fscoreHeap[idx1].heap_idx = idx1;
    fscoreHeap[idx2].heap_idx = idx2;

    // insert updated heap values to open list
    openList.erase(fscoreHeap[idx1]);
    openList.insert(fscoreHeap[idx1]);

    openList.erase(fscoreHeap[idx2]);
    openList.insert(fscoreHeap[idx2]);
}

node_ AStarOpenList::removeMinimum() {
    exchange(0, size-1);
    node_ fscore_deleted = fscoreHeap[size-1];
    fscoreHeap.pop_back();
    size--;
    sink(0);
    openList.erase(fscore_deleted);
    return fscore_deleted;
}

float AStarOpenList::peekMinimum() {
    return fscoreHeap[0].getFScore();
}

bool AStarOpenList::isEmpty() {
    return size == 0;
}

bool AStarOpenList::isPresent(node_& n) {
    return openList.find(n) != openList.end();
}

bool AStarOpenList::updateIfBetterPath(node_& n, float gvalue) {
    auto t = openList.find(n);
    if (gvalue < t->g) {
        n.computeF(gvalue, t->h);
        n.heap_idx = t->heap_idx;
        fscoreHeap[t->heap_idx] = n;
        openList.erase(n);
        openList.insert(n);
        swim(n.heap_idx);
        return true;
    }
    return false;
}

int AStarOpenList::getMaxSize() {
    return maxSize;
}
