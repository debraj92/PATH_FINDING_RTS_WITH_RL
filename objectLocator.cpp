//
// Created by Debraj Ray on 2022-01-07.
//

#include "objectLocator.h"
#include "gameConstants.h"
#include <cmath>
#include <iostream>

using namespace std;


void objectLocator::init() {
    l1[SE] = 1 / sqrtf(2);
    l2[SE] = 1 / sqrtf(2);
    p1[SE] = - 1 / sqrtf(2);
    p2[SE] = 1 / sqrtf(2);

    l1[N] = -1;
    l2[N] = 0;
    p1[N] = 0;
    p2[N] = -1;

    l1[NE] = -1 / sqrtf(2);
    l2[NE] = 1 / sqrtf(2);
    p1[NE] = -1 / sqrtf(2);
    p2[NE] = -1 / sqrtf(2);

    l1[NW] = -1 / sqrtf(2);
    l2[NW] = -1 / sqrtf(2);
    p1[NW] = 1 / sqrtf(2);
    p2[NW] = -1 / sqrtf(2);

    l1[E] = 0;
    l2[E] = 1;
    p1[E] = -1;
    p2[E] = 0;

    l1[W] = 0;
    l2[W] = -1;
    p1[W] = 1;
    p2[W] = 0;

    l1[S] = 1;
    l2[S] = 0;
    p1[S] = 0;
    p2[S] = 1;

    l1[SW] = 1 / sqrtf(2);
    l2[SW] = -1 / sqrtf(2);
    p1[SW] = 1 / sqrtf(2);
    p2[SW] = 1 / sqrtf(2);
}

void objectLocator::locateObject(int player_x, int player_y, int direction, int object_x, int object_y) {
    logger->logDebug("Unit direction")->logDebug(direction)->endLineDebug();
    logger->logDebug("Object position: ")->logDebug(object_x)->logDebug(", ")->logDebug(object_y)->endLineDebug();
    float det = calculateDeterminant(direction);
    int diff1 = object_x - player_x;
    int diff2 = object_y - player_y;
    object_l = (p2[direction] * static_cast< float >(diff1) - p1[direction] * static_cast< float >(diff2)) / det;
    object_p = (-l2[direction] * static_cast< float >(diff1) + l1[direction] * static_cast< float >(diff2)) / det;

    computeDistance(diff1, diff2);
}

float objectLocator::calculateDeterminant(int direction) {
    return l1[direction]*p2[direction] - p1[direction]*l2[direction];
}

void objectLocator::computeDistance(int x_diff, int y_diff) {
    object_radius = sqrtf(powf(object_l, 2) + powf(object_p, 2));
    object_walking_distance = max(abs(x_diff), abs(y_diff));
    logger->logDebug("object distance ")->logDebug(object_radius)->endLineDebug();
    logger->logDebug("object distance R ")->logDebug(object_walking_distance)->endLineDebug();
}

void objectLocator::computeCosine() {
    if(object_radius != 0) {
        object_angle = object_l / object_radius;
    }
}

void objectLocator::computeSine() {
    if(object_radius != 0) {
        object_angle = object_p / object_radius;
    }
}

float objectLocator::getObjectDistance() {
    logger->logDebug("getObjectDistance")->endLineDebug();
    return object_walking_distance;
}

float objectLocator::round_values(float value) {
    // round to 2 decimal places
    return std::round(value * 100) / 100;
}

void objectLocator::findQuadrant() {
    if (object_l >= 0 and object_p >= 0) {
        quadrant = 1;
    } else if (object_p >= 0) {
        quadrant = 2;
    } else if (object_l >= 0) {
        quadrant = 4;
    } else {
        quadrant = 3;
    }
    logger->logDebug("Quadrant")->endLineDebug();
}

void objectLocator::measureUniqueAngle() {
    switch (quadrant) {
        case 1:
            computeCosine();
            object_angle++;
            break;
        case 2:
        case 3:
            computeSine();
            break;
        case 4:
            computeCosine();
            object_angle *= -1;
            object_angle--;
            break;
    }
    logger->logDebug("measureUniqueAngle ")->logDebug(object_angle)->endLineDebug();
}

void objectLocator::calculateRiskFromDistance() {
    risk_distance = static_cast<float> (exp(-object_walking_distance * 0.9)); // 1.1 if distance to be given more weight
    logger->logDebug("calculateRiskFromDistance ")->logDebug(risk_distance)->endLineDebug();
}

void objectLocator::calculateRiskFeatures() {
    risk_feature = risk_distance * abs(object_angle);
    logger->logDebug("calculateRiskFeatures ")->logDebug(risk_feature)->endLineDebug();
}

float objectLocator::getObjectRiskFeature() {
    logger->logDebug("objectLocator::getObjectRiskFeature")->endLineDebug();
    return risk_feature;
}

float objectLocator::getObjectAngle() {
    logger->logDebug("objectLocator::getObjectAngle")->endLineDebug();
    return object_angle;
}

void objectLocator::measureRiskAndObjectAngle() {
    logger->logDebug("objectLocator::measureRiskAndObjectAngle")->endLineDebug();
    findQuadrant();
    measureUniqueAngle();
    calculateRiskFromDistance();
    calculateRiskFeatures();
}

float objectLocator::dotDirectionVector(int direction, float vector_x, float vector_y) {
    return l1[direction] * vector_x + l2[direction] * vector_y;
}