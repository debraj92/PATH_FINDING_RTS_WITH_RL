//
// Created by Debraj Ray on 2022-08-12.
//

#include <gtest/gtest.h>
#include "../AStar_.h"
#include "../findPath.h"
#include "../gameConstants.h"
#include "../fixedobstacles.h"
#include "../enemy/enemy.h"


using namespace std;
using namespace RTS;

/// set GRID_SPAN = 12

void Abstract_AStarChase_createAllFixedObstacles(int TOTAL_FIXED_OBSTACLES, int blockObstacles[][4], vector<std::vector<int>> &grid) {

    // fill with static obstacles
    FixedObstacles fixedObstacles;
    for(int obstacle=0; obstacle<TOTAL_FIXED_OBSTACLES; obstacle++) {
        int x_s = blockObstacles[obstacle][0];
        int x_e = blockObstacles[obstacle][1];
        int y_s = blockObstacles[obstacle][2];
        int y_e = blockObstacles[obstacle][3];
        fixedObstacles.createBlockObstacle(x_s, x_e, y_s, y_e, grid);
    }
}

TEST(AbstractAStar, deferredFindPath) {

    vector<vector<int>> grid;
    for (int i=0; i<GRID_SPAN; i++) {
        std::vector<int> row(GRID_SPAN, 0);
        grid.push_back(row);
    }
    int x = 0;
    int y = 0;
    findPath fp(grid, x, y, GRID_SPAN - 1, GRID_SPAN - 1);
    fp.findPathToDestinationDeferred();
    while(x != GRID_SPAN - 1 or y != GRID_SPAN - 1) {
        cout<<"At ("<<x<<","<<y<<")"<<endl;
        cout<<"is on track? "<<fp.isOnTrack(x, y)<<endl;
        fp.calculateNextPosition();
        x = fp.getNext_x();
        y = fp.getNext_y();
    }
    cout<<"At ("<<x<<","<<y<<")"<<endl;

}

TEST(AbstractAStar, deferredFindPathWithObstacles) {

    vector<vector<int>> grid;
    for (int i=0; i<GRID_SPAN; i++) {
        std::vector<int> row(GRID_SPAN, 0);
        grid.push_back(row);
    }

    const int TOTAL_FIXED_OBSTACLES = 2;
    int blockObstacles[TOTAL_FIXED_OBSTACLES][4] = {
            //x_s, x_e, y_s, y_e
            {5, 6, 5, 7},
            {1, 2, 1, 2}
    };
    Logger::GLOBAL_LOG_LEVEL = RTS::DEBUG;
    Logger logger(LOG_LEVEL::DEBUG);


    Abstract_AStarChase_createAllFixedObstacles(TOTAL_FIXED_OBSTACLES, blockObstacles, grid);
    logger.printBoardInfo(grid);

    int x = 0;
    int y = 0;
    findPath fp(grid, x, y, GRID_SPAN - 1, GRID_SPAN - 1);
    fp.findPathToDestinationDeferred();
    while(x != GRID_SPAN - 1 or y != GRID_SPAN - 1) {
        cout<<"At ("<<x<<","<<y<<")"<<endl;
        cout<<"is on track? "<<fp.isOnTrack(x, y)<<endl;
        fp.calculateNextPosition();
        x = fp.getNext_x();
        y = fp.getNext_y();
    }
    cout<<"At ("<<x<<","<<y<<")"<<endl;


}


TEST(AbstractAStar, stitchedPath) {

    vector<vector<int>> grid;
    for (int i=0; i<GRID_SPAN; i++) {
        std::vector<int> row(GRID_SPAN, 0);
        grid.push_back(row);
    }
    int x = 0;
    int y = 0;
    findPath fp(grid, x, y, GRID_SPAN - 1, GRID_SPAN - 1);
    fp.findPathToDestination();

    // (5,5) is on track
    x = 7;
    y = 8;
    findPath fp2(grid, x, y, 5, 5);
    fp2.findPathToDestination();
    fp.stitchNewPathIntoExistingAtNode(fp2, 5, 5, x, y);

    while(x != GRID_SPAN - 1 or y != GRID_SPAN - 1) {
        cout<<"At ("<<x<<","<<y<<")"<<endl;
        cout<<"is on track? "<<fp.isOnTrack(x, y)<<endl;
        fp.calculateNextPosition();
        x = fp.getNext_x();
        y = fp.getNext_y();
    }
    cout<<"At ("<<x<<","<<y<<")"<<endl;

}

TEST(AbstractAStar, stitchedPathOldLocationLoop) {

    Logger logger1(LOG_LEVEL::INFO);

    vector<vector<int>> grid;
    for (int i=0; i<GRID_SPAN; i++) {
        std::vector<int> row(GRID_SPAN, 0);
        grid.push_back(row);
    }
    const int TOTAL_FIXED_OBSTACLES = 1;
    int blockObstacles[TOTAL_FIXED_OBSTACLES][4] = {
            //x_s, x_e, y_s, y_e
            {5, 8, 4, 6}
    };
    Logger logger(LOG_LEVEL::INFO);

    Abstract_AStarChase_createAllFixedObstacles(TOTAL_FIXED_OBSTACLES, blockObstacles, grid);
    logger.printBoardInfo(grid);

    int x = GRID_SPAN - 1;
    int y = 0;
    findPath fp(grid, x, y, 0, GRID_SPAN - 1);
    fp.findPathToDestination();

    cout<<"Re-route branch"<<endl;

    x = 5;
    y = 7;
    findPath fp2(grid, x, y, GRID_SPAN - 2, 1);
    fp2.findPathToDestination();

    cout<<"stitching"<<endl;
    fp.stitchNewPathIntoExistingAtNode(fp2, GRID_SPAN - 2, 1, x, y);

    while(x != 0 or y != GRID_SPAN - 1) {
        cout<<"At ("<<x<<","<<y<<")"<<endl;
        cout<<"is on track? "<<fp.isOnTrack(x, y)<<endl;
        fp.calculateNextPosition();
        x = fp.getNext_x();
        y = fp.getNext_y();
    }
    cout<<"At ("<<x<<","<<y<<")"<<endl;

}

TEST(AbstractAStar, stitchedPathDestination) {

    Logger logger1(LOG_LEVEL::INFO);

    vector<vector<int>> grid;
    for (int i=0; i<GRID_SPAN; i++) {
        std::vector<int> row(GRID_SPAN, 0);
        grid.push_back(row);
    }

    int x = GRID_SPAN - 1;
    int y = 0;
    findPath fp(grid, x, y, 0, GRID_SPAN - 1);
    fp.findPathToDestination();

    cout<<"Re-route branch"<<endl;

    x = 5;
    y = 7;
    findPath fp2(grid, x, y, 0, GRID_SPAN - 1);
    fp2.findPathToDestination();

    cout<<"stitching"<<endl;
    fp.stitchNewPathIntoExistingAtNode(fp2, 0, GRID_SPAN - 1, x, y);

    while(x != 0 or y != GRID_SPAN - 1) {
        cout<<"At ("<<x<<","<<y<<")"<<endl;
        cout<<"is on track? "<<fp.isOnTrack(x, y)<<endl;
        fp.calculateNextPosition();
        x = fp.getNext_x();
        y = fp.getNext_y();
    }
    cout<<"At ("<<x<<","<<y<<")"<<endl;

}