//
// Created by Debraj Ray on 2022-01-03.
//


#include "state.h"
#include <iostream>

using namespace std;

unsigned long state::getMaxQValueAction() {
    printPolicy();
    return argmax(policy)(0);
}

void state::updateActionQvalue(int action, double q_value) {
    policy(action) = q_value;
    printData();
    printPolicy();
}

double state::getQvalueOfAction(int action) {
    //cout<<"state::getQvalueOfAction"<<endl;
    //cout<<"action"<<action<<endl;
    //printPolicy();
    return policy.at(action);
}

double state::getMaxProbability() {
    return amax(policy)(0);
}

void state::printPolicy() {
    cout<<"X,Y ="<<x<<", "<<y<<endl;
    cout<<"Policy Size"<<policy.size()<<endl;
    cout<<"ACTION_FOLLOW: "<<policy.at(ACTION_FOLLOW)<<endl;
    cout<<"ACTION_DODGE_LEFT: "<<policy.at(ACTION_DODGE_LEFT)<<endl;
    cout<<"ACTION_DODGE_DIAGONAL_LEFT: "<<policy.at(ACTION_DODGE_DIAGONAL_LEFT)<<endl;
    cout<<"ACTION_DODGE_RIGHT: "<<policy.at(ACTION_DODGE_RIGHT)<<endl;
    cout<<"ACTION_DODGE_DIAGONAL_RIGHT: "<<policy.at(ACTION_DODGE_DIAGONAL_RIGHT)<<endl;
    cout<<"ACTION_STRAIGHT: "<<policy.at(ACTION_STRAIGHT)<<endl;
    cout<<"ACTION_REROUTE: "<<policy.at(ACTION_REROUTE)<<endl;
    cout<<"ACTION_SWITCH: "<<policy.at(ACTION_SWITCH)<<endl;
}




