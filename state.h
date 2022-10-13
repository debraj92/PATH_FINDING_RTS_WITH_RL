//
// Created by Debraj Ray on 2021-12-31.
//

#ifndef EXAMPLE_STATE_H
#define EXAMPLE_STATE_H

#include "gameConstants.h"
#include "observation.h"
#include "coordinatesUtil.h"

#include <xtensor/xrandom.hpp>
#include <xtensor/xarray.hpp>
#include <xtensor/xnpy.hpp>
#include <xtensor/xsort.hpp>
#include <xtensor/xtensor.hpp>
#include <xtensor/xio.hpp>

#include <cmath>

using namespace xt;

class state : public observation {

    xarray<double> policy;

public:

    // TODO: FOR DEBUGGING
    int x, y;

    state () {
        xarray<double>::shape_type shape = {ACTION_SPACE};
        policy = xarray<double>::from_shape(shape);
        policy.fill(0);
    }

    void printPolicy();

    unsigned long getMaxQValueAction();

    void updateActionQvalue(int action, double q_value);

    double getQvalueOfAction(int action);

    double getMaxProbability();

};


#endif //EXAMPLE_STATE_H
