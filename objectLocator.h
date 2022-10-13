//
// Created by Debraj Ray on 2022-01-07.
//

#ifndef EXAMPLE_OBJECTLOCATOR_H
#define EXAMPLE_OBJECTLOCATOR_H

#include <map>
#include "gameConstants.h"
#include "Logger.h"
#include <cmath>


using namespace RTS;
using namespace std;

class objectLocator {

    const LOG_LEVEL LogLevel = LOG_LEVEL::INFO;
    const bool SET_COMPONENT_LOG_OFF = true;

    std::unique_ptr<Logger> logger;

    float l1[9];
    float l2[9];
    float p1[9];
    float p2[9];

    float object_l;
    float object_p;
    float object_radius;
    float object_walking_distance;
    float object_angle;
    int quadrant;
    float risk_distance;

    float risk_feature;

    void init();

    float calculateDeterminant(int direction);

    void computeDistance(int x_diff, int y_diff);

    void computeCosine();

    void computeSine();

    void findQuadrant();

    void measureUniqueAngle(); // psi

    void calculateRiskFromDistance();

    void calculateRiskFeatures();

    static float round_values(float value);

public:

    objectLocator() {
        init();
        logger = std::make_unique<Logger>(LogLevel);
        logger->turnLogOff = SET_COMPONENT_LOG_OFF;
    }

    void locateObject(int player_x, int player_y, int direction, int object_x, int object_y);

    void measureRiskAndObjectAngle();

    float getObjectDistance();

    float getObjectAngle();

    float getObjectRiskFeature();

    float dotDirectionVector(int direction, float vector_x, float vector_y);
};


#endif //EXAMPLE_OBJECTLOCATOR_H
