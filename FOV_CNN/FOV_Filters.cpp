//
// Created by Debraj Ray on 2022-06-26.
//

#include "FOV_Filters.h"
#include <iostream>

using namespace std;
void FOV_Filters::init() {

    // N filter
    for(int i=0; i<FOV_WIDTH; i++) {
        for(int j=0; j<FOV_WIDTH; j++) {
            direction_mapping[N][i][j] = {i, j};
        }
    }

    // S filter
    int x=0, y=0;
    for(int i=FOV_WIDTH; i>=0; i--) {
        y=0;
        for(int j=FOV_WIDTH; j>=0; j--) {
            direction_mapping[S][x][y] = {i-1, j-1};
            y++;
        }
        x++;
    }

    // E filter
    createFilterE();

    // W filter
    createFilterW();

    createFilterNE();
    createFilterSE();
    createFilterSW();
    createFilterNW();

}

void FOV_Filters::createFilterNE() {
    for(int i=0; i<FOV_WIDTH; i++) {
        for(int j=0; j<FOV_WIDTH; j++) {
            direction_mapping[NE][i][j] = {i, j};
        }
    }
    for(int ring = 1; ring <= VISION_RADIUS; ++ring) {
        for(int rotationCount = 1; rotationCount <= ring; ++rotationCount) {
            rotateFilter(direction_mapping[NE], ring);
        }
    }
}

void FOV_Filters::createFilterSE() {
    for(int i=0; i<FOV_WIDTH; i++) {
        for(int j=0; j<FOV_WIDTH; j++) {
            direction_mapping[SE][i][j] = {i, j};
        }
    }
    for(int ring = 1; ring <= VISION_RADIUS; ++ring) {
        for(int rotationCount = 1; rotationCount <= ring * 3; ++rotationCount) {
            rotateFilter(direction_mapping[SE], ring);
        }
    }
}

void FOV_Filters::createFilterSW() {
    for(int i=0; i<FOV_WIDTH; i++) {
        for(int j=0; j<FOV_WIDTH; j++) {
            direction_mapping[SW][i][j] = {i, j};
        }
    }
    for(int ring = 1; ring <= VISION_RADIUS; ++ring) {
        for(int rotationCount = 1; rotationCount <= ring * 5; ++rotationCount) {
            rotateFilter(direction_mapping[SW], ring);
        }
    }
}

void FOV_Filters::createFilterNW() {
    for(int i=0; i<FOV_WIDTH; i++) {
        for(int j=0; j<FOV_WIDTH; j++) {
            direction_mapping[NW][i][j] = {i, j};
        }
    }
    for(int ring = 1; ring <= VISION_RADIUS; ++ring) {
        for(int rotationCount = 1; rotationCount <= ring * 7; ++rotationCount) {
            rotateFilter(direction_mapping[NW], ring);
        }
    }
}

void FOV_Filters::createFilterE() {
    for(int i=0; i<FOV_WIDTH; i++) {
        for(int j=0; j<FOV_WIDTH; j++) {
            direction_mapping[E][i][j] = {i, j};
        }
    }
    for(int ring = 1; ring <= VISION_RADIUS; ++ring) {
        for(int rotationCount = 1; rotationCount <= ring * 2; ++rotationCount) {
            rotateFilter(direction_mapping[E], ring);
        }
    }
}

void FOV_Filters::createFilterW() {
    for(int i=0; i<FOV_WIDTH; i++) {
        for(int j=0; j<FOV_WIDTH; j++) {
            direction_mapping[W][i][j] = {i, j};
        }
    }
    for(int ring = 1; ring <= VISION_RADIUS; ++ring) {
        for(int rotationCount = 1; rotationCount <= ring * 6; ++rotationCount) {
            rotateFilter(direction_mapping[W], ring);
        }
    }
}

void FOV_Filters::rotateFilter (gridLocation (&directionFilter)[FOV_WIDTH][FOV_WIDTH], int ring) {

    int x=VISION_RADIUS-ring, y=VISION_RADIUS-ring;
    int v = ring;
    int i = -v + 1, j = -v;
    x++;
    gridLocation directionFilter_copy[FOV_WIDTH][FOV_WIDTH];
    copy(&directionFilter[0][0], &directionFilter[0][0] + FOV_WIDTH * FOV_WIDTH, &directionFilter_copy[0][0]);
    while (i <= v) {
        directionFilter[x][y] = {directionFilter_copy[x - 1][y].x, directionFilter_copy[x - 1][y].y};
        i++;
        x++;
    }
    i--;
    x--;
    j++;
    y++;
    while (j <= v) {
        directionFilter[x][y] = {directionFilter_copy[x][y-1].x, directionFilter_copy[x][y-1].y};
        j++;
        y++;
    }
    j--;
    y--;
    i--;
    x--;
    while (i >= -v) {
        directionFilter[x][y] = {directionFilter_copy[x + 1][y].x, directionFilter_copy[x + 1][y].y};
        x--;
        i--;
    }
    i++;
    x++;
    j--;
    y--;
    while (j >= -v) {
        directionFilter[x][y] = {directionFilter_copy[x][y+1].x, directionFilter_copy[x][y+1].y};
        y--;
        j--;
    }
}


