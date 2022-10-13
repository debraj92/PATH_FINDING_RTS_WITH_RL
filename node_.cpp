//
// Created by Debraj Ray on 2022-02-13.
//

#include "node_.h"


void node_::computeF(float g, float h, float r) {
    this->g = g;
    this->h = h;
    this->r = r;
    f = g + h + r;
}

float node_::getFScore() {
    return f;
}


