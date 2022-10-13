//
// Created by Debraj Ray on 2022-06-26.
//

#ifndef EXAMPLE_TESTS_FOV_FILTERS_H
#define EXAMPLE_TESTS_FOV_FILTERS_H

#include "../gameConstants.h"

class FOV_Filters {

    void createFilterNE();

    void createFilterSE();

    void createFilterSW();

    void createFilterNW();

    void createFilterE();

    void createFilterW();

public:
    FOV_Filters() {
    }
    struct gridLocation{
        int x, y;
    };

    gridLocation direction_mapping[9][FOV_WIDTH][FOV_WIDTH];

    void rotateFilter(gridLocation (&directionFilter)[FOV_WIDTH][FOV_WIDTH], int ring);

    void init();

};


#endif //EXAMPLE_TESTS_FOV_FILTERS_H
