//
// Created by Debraj Ray on 2022-06-26.
//

#include <gtest/gtest.h>
#include <vector>
#include "../gameConstants.h"
#include "../FOV_CNN/FOV_Filters.h"
#include <iostream>

using namespace std;

void printMatrix(FOV_Filters::gridLocation (&matrix)[FOV_WIDTH][FOV_WIDTH]) {
    for(int i=0; i<FOV_WIDTH; i++) {
        for(int j=0; j<FOV_WIDTH; j++) {
            cout<<"("<<matrix[i][j].x<<","<<matrix[i][j].y<<") ";
        }
        cout<<endl;
    }
}

TEST(FOV_FILTER, TestRotate1) {
    FOV_Filters::gridLocation matrix[FOV_WIDTH][FOV_WIDTH];
    for(int i=0; i<FOV_WIDTH; i++) {
        for(int j=0; j<FOV_WIDTH; j++) {
            matrix[i][j] = {i, j};
        }
    }
    printMatrix(matrix);
    FOV_Filters ff;
    ff.rotateFilter(matrix, 1);
    cout<<"Rotated ring1: "<<endl;
    printMatrix(matrix);

    ff.rotateFilter(matrix, 1);
    cout<<"Rotated Again ring1: "<<endl;
    printMatrix(matrix);

    ff.rotateFilter(matrix, 2);
    cout<<"Rotated ring2: "<<endl;
    printMatrix(matrix);

    ff.rotateFilter(matrix, 2);
    cout<<"Rotated Again ring2: "<<endl;
    printMatrix(matrix);

}

TEST(FOV_FILTER, DIRECTION_FILTER_NE) {
    FOV_Filters::gridLocation matrix[FOV_WIDTH][FOV_WIDTH];
    for(int i=0; i<FOV_WIDTH; i++) {
        for(int j=0; j<FOV_WIDTH; j++) {
            matrix[i][j] = {i, j};
        }
    }

    printMatrix(matrix);
    FOV_Filters ff;
    ff.init();
    cout<<"NE FOV"<<endl;
    printMatrix(ff.direction_mapping[NE]);
}

TEST(FOV_FILTER, DIRECTION_FILTER_SE) {
    FOV_Filters::gridLocation matrix[FOV_WIDTH][FOV_WIDTH];
    for(int i=0; i<FOV_WIDTH; i++) {
        for(int j=0; j<FOV_WIDTH; j++) {
            matrix[i][j] = {i, j};
        }
    }

    printMatrix(matrix);
    FOV_Filters ff;
    ff.init();
    cout<<"SE FOV"<<endl;
    printMatrix(ff.direction_mapping[SE]);
}

TEST(FOV_FILTER, DIRECTION_FILTER_SW) {
    FOV_Filters::gridLocation matrix[FOV_WIDTH][FOV_WIDTH];
    for(int i=0; i<FOV_WIDTH; i++) {
        for(int j=0; j<FOV_WIDTH; j++) {
            matrix[i][j] = {i, j};
        }
    }

    printMatrix(matrix);
    FOV_Filters ff;
    ff.init();
    cout<<"SW FOV"<<endl;
    printMatrix(ff.direction_mapping[SW]);
}

TEST(FOV_FILTER, DIRECTION_FILTER_NW) {
    FOV_Filters::gridLocation matrix[FOV_WIDTH][FOV_WIDTH];
    for(int i=0; i<FOV_WIDTH; i++) {
        for(int j=0; j<FOV_WIDTH; j++) {
            matrix[i][j] = {i, j};
        }
    }

    printMatrix(matrix);
    FOV_Filters ff;
    ff.init();
    cout<<"SW FOV"<<endl;
    printMatrix(ff.direction_mapping[NW]);
}


TEST(FOV_FILTER, DIRECTION_FILTER_N_S_E_W) {
    FOV_Filters::gridLocation matrix[FOV_WIDTH][FOV_WIDTH];
    for(int i=0; i<FOV_WIDTH; i++) {
        for(int j=0; j<FOV_WIDTH; j++) {
            matrix[i][j] = {i, j};
        }
    }

    printMatrix(matrix);

    FOV_Filters ff;
    ff.init();
    cout<<"N FOV"<<endl;
    printMatrix(ff.direction_mapping[N]);

    cout<<"E FOV"<<endl;
    printMatrix(ff.direction_mapping[E]);

    cout<<"S FOV"<<endl;
    printMatrix(ff.direction_mapping[S]);

    cout<<"W FOV"<<endl;
    printMatrix(ff.direction_mapping[W]);
}
