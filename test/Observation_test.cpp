//
// Created by Debraj Ray on 2022-05-09.
//

#include <gtest/gtest.h>
#include <vector>
#include <iostream>

#include "../fixedobstacles.h"
#include "../observation.h"

using namespace std;

/**
 * SET GRID_SPAN to 10
 * SET VISION RADIUS to 3
 * Requires un-smoothened A*
 */

TEST(ObservationDirectionTrajectory, BasicAssertions) {

    vector<vector<int>> grid;
    for (int i=0; i<GRID_SPAN; i++) {
        std::vector<int> row(GRID_SPAN, 0);
        grid.push_back(row);
    }
    int current_x = 0, current_y = 0, dest_x = 9, dest_y = 9;
    auto fp = std::make_shared<findPath>(grid, current_x, current_y, dest_x, dest_y);
    fp->findPathToDestination();

    observation ob;
    ob.playerX = current_x;
    ob.playerY = current_y;
    ob.locateTrajectoryAndDirection(fp);
    assert(ob.direction == SE);
    assert(ob.trajectory == on_track);

    const int TOTAL_FIXED_OBSTACLES = 1;
    int blockObstacles[TOTAL_FIXED_OBSTACLES][4] = {
            //x_s, x_e, y_s, y_e
            {0, 4, 1, 4}

    };

    // fill with static obstacles
    FixedObstacles fixedObstacles;
    for(int obstacle=0; obstacle<TOTAL_FIXED_OBSTACLES; obstacle++) {
        int x_s = blockObstacles[obstacle][0];
        int x_e = blockObstacles[obstacle][1];
        int y_s = blockObstacles[obstacle][2];
        int y_e = blockObstacles[obstacle][3];
        fixedObstacles.createBlockObstacle(x_s, x_e, y_s, y_e, grid);
    }
    fp->changeMap(grid);
    fp->findPathToDestination();

    ob.direction = 0;
    ob.trajectory = 0;
    ob.playerX = 5;
    ob.playerY = 2;
    ob.locateTrajectoryAndDirection(fp);
    ob.locateRelativeTrajectory();
    assert(ob.direction == SE);
    assert(ob.trajectory == one_deviation_S);
    assert(ob.trajectory_right);

    ob.direction = 0;
    ob.trajectory = 0;
    ob.playerX = 5;
    ob.playerY = 3;
    ob.locateTrajectoryAndDirection(fp);
    ob.locateRelativeTrajectory();
    assert(ob.direction == SE);
    assert(ob.trajectory == one_deviation_S);
    assert(ob.trajectory_right);

    ob.direction = 0;
    ob.trajectory = 0;
    ob.playerX = 5;
    ob.playerY = 4;
    ob.locateTrajectoryAndDirection(fp);
    ob.locateRelativeTrajectory();

    assert(ob.direction == SE);
    assert(ob.trajectory == two_deviation_S);
    assert(ob.trajectory_right);

    ob.direction = 0;
    ob.trajectory = 0;
    ob.playerX = 8;
    ob.playerY = 5;
    ob.locateTrajectoryAndDirection(fp);
    ob.locateRelativeTrajectory();

    assert(ob.direction == E);
    assert(ob.trajectory == one_deviation_S);
    assert(ob.trajectory_right);

    ob.direction = 0;
    ob.trajectory = 0;
    ob.playerX = 9;
    ob.playerY = 2;
    ob.locateTrajectoryAndDirection(fp);
    ob.locateRelativeTrajectory();

    assert(ob.direction == SE);
    assert(ob.trajectory == two_deviation_E);
    assert(ob.trajectory_left == 2);

    fp->changeSourceAndDestination(0, 5, 5, 0);
    fp->findPathToDestination();

    ob.direction = 0;
    ob.trajectory = 0;
    ob.playerX = 3;
    ob.playerY = 6;
    ob.locateTrajectoryAndDirection(fp);

    assert(ob.direction == S);
    assert(ob.trajectory == one_deviation_W);

    ob.locateRelativeTrajectory();
    assert(ob.trajectory_right == 1);
    assert(ob.trajectory_left == 0);
    assert(ob.trajectory_front == 0);
    assert(ob.trajectory_on_track == 0);
    assert(ob.trajectory_off_track == 0);

    ob.direction = 0;
    ob.trajectory = 0;
    ob.playerX = 8;
    ob.playerY = 8;
    ob.locateTrajectoryAndDirection(fp);
    ob.locateRelativeTrajectory();

    assert(ob.direction == 0);
    assert(ob.trajectory == 0);
    assert(ob.trajectory_off_track);

}

TEST(ObservationRelativeTrajectory, BasicAssertions) {
    observation ob;
    ob.trajectory = on_track;
    for (int d=1; d<=8; d++) {
        ob.direction = d;
        ob.locateRelativeTrajectory();

        assert(ob.trajectory_on_track == 1);
        assert(ob.trajectory_left == 0);
        assert(ob.trajectory_right == 0);
        assert(ob.trajectory_front == 0);
        assert(ob.trajectory_off_track == 0);
    }

    ob = observation();
    ob.trajectory = 11;
    ob.direction = NE;
    ob.locateRelativeTrajectory();
    assert(ob.trajectory_on_track == 0);
    assert(ob.trajectory_left == 1);
    assert(ob.trajectory_right == 0);
    assert(ob.trajectory_front == 0);
    assert(ob.trajectory_off_track == 0);

    ob = observation();
    ob.trajectory = 13;
    ob.direction = SW;
    ob.locateRelativeTrajectory();

    assert(ob.trajectory_on_track == 0);
    assert(ob.trajectory_left == 0);
    assert(ob.trajectory_right == 1);
    assert(ob.trajectory_front == 0);
    assert(ob.trajectory_off_track == 0);

    ob = observation();
    ob.trajectory = 22;
    ob.direction = W;
    ob.locateRelativeTrajectory();

    assert(ob.trajectory_on_track == 0);
    assert(ob.trajectory_left == 2);
    assert(ob.trajectory_right == 0);
    assert(ob.trajectory_front == 0);
    assert(ob.trajectory_off_track == 0);

    ob = observation();
    ob.trajectory = 24;
    ob.direction = NE;
    ob.locateRelativeTrajectory();

    assert(ob.trajectory_on_track == 0);
    assert(ob.trajectory_left == 0);
    assert(ob.trajectory_right == 2);
    assert(ob.trajectory_front == 0);
    assert(ob.trajectory_off_track == 0);

    ob = observation();
    ob.trajectory = 24;
    ob.direction = E;
    ob.locateRelativeTrajectory();

    assert(ob.trajectory_on_track == 0);
    assert(ob.trajectory_left == 0);
    assert(ob.trajectory_right == 0);
    assert(ob.trajectory_front == 2);
    assert(ob.trajectory_off_track == 0);

    ob = observation();
    ob.trajectory = 12;
    ob.direction = S;
    ob.locateRelativeTrajectory();

    assert(ob.trajectory_on_track == 0);
    assert(ob.trajectory_left == 0);
    assert(ob.trajectory_right == 0);
    assert(ob.trajectory_front == 1);
    assert(ob.trajectory_off_track == 0);

    ob = observation();
    ob.trajectory = 0;
    ob.direction = 0;
    ob.locateRelativeTrajectory();
    assert(ob.trajectory_on_track == 0);
    assert(ob.trajectory_left == 0);
    assert(ob.trajectory_right == 0);
    assert(ob.trajectory_front == 0);
    assert(ob.trajectory_off_track == 1);

}

TEST(ObservationDirectionTrajectory2, BasicAssertions) {
    vector<vector<int>> grid;
    for (int i=0; i<GRID_SPAN; i++) {
        std::vector<int> row(GRID_SPAN, 0);
        grid.push_back(row);
    }

    const int TOTAL_FIXED_OBSTACLES = 4;
    int blockObstacles[TOTAL_FIXED_OBSTACLES][4] = {
            //x_s, x_e, y_s, y_e
            {0, 2, 0, 0},
            {0, 0, 1, 1},
            {2, 4, 2, 2},
            {2, 2, 3, 3},

    };

    // fill with static obstacles
    FixedObstacles fixedObstacles;
    for(int obstacle=0; obstacle<TOTAL_FIXED_OBSTACLES; obstacle++) {
        int x_s = blockObstacles[obstacle][0];
        int x_e = blockObstacles[obstacle][1];
        int y_s = blockObstacles[obstacle][2];
        int y_e = blockObstacles[obstacle][3];
        fixedObstacles.createBlockObstacle(x_s, x_e, y_s, y_e, grid);
    }

    auto fp = std::make_shared<findPath>(grid, 2, 4, 3, 0);
    assert (fp->findPathToDestination());
    observation ob;
    ob.playerX = 1;
    ob.playerY = 4;
    ob.locateTrajectoryAndDirection(fp);
    ob.locateRelativeTrajectory();
    assert(ob.direction == W);
    assert(ob.trajectory_front == 1);
    assert(ob.countNodeNumbersInDirection == 3);

    ob.playerX = 0;
    ob.playerY = 3;
    ob.trajectory = 0;
    ob.direction = 0;
    ob.locateTrajectoryAndDirection(fp);
    ob.locateRelativeTrajectory();
    assert(ob.direction == SW);
    assert(ob.trajectory_left == 1);
    assert(ob.countNodeNumbersInDirection == 4);

    ob.playerX = 1;
    ob.playerY = 1;
    ob.trajectory = 0;
    ob.direction = 0;
    ob.locateTrajectoryAndDirection(fp);
    ob.locateRelativeTrajectory();
    assert(ob.direction == SW);
    assert(ob.trajectory_left == 1);
    assert(ob.countNodeNumbersInDirection == 5);

    ob.playerX = 3;
    ob.playerY = 1;
    ob.trajectory = 0;
    ob.direction = 0;
    ob.locateTrajectoryAndDirection(fp);
    ob.locateRelativeTrajectory();
    assert(ob.direction == SW);
    assert(ob.trajectory_right == 1);
    assert(ob.countNodeNumbersInDirection == 5);

}

TEST(ObservationDirectionTrajectory3, BasicAssertions) {

    vector<vector<int>> grid;
    for (int i=0; i<GRID_SPAN; i++) {
        std::vector<int> row(GRID_SPAN, 0);
        grid.push_back(row);
    }

    const int TOTAL_FIXED_OBSTACLES = 2;
    int blockObstacles[TOTAL_FIXED_OBSTACLES][4] = {
            //x_s, x_e, y_s, y_e
            {1, 2, 5, 7},
            {3, 4, 3, 5}

    };

    // fill with static obstacles
    FixedObstacles fixedObstacles;
    for(int obstacle=0; obstacle<TOTAL_FIXED_OBSTACLES; obstacle++) {
        int x_s = blockObstacles[obstacle][0];
        int x_e = blockObstacles[obstacle][1];
        int y_s = blockObstacles[obstacle][2];
        int y_e = blockObstacles[obstacle][3];
        fixedObstacles.createBlockObstacle(x_s, x_e, y_s, y_e, grid);
    }

    auto fp = std::make_shared<findPath>(grid, 0, 6, 3, 6);
    assert (fp->findPathToDestination());
    observation ob;
    ob.playerX = 0;
    ob.playerY = 9;
    ob.locateTrajectoryAndDirection(fp);
    ob.locateRelativeTrajectory();
    assert(ob.direction == S);
    assert(ob.trajectory_front == 1);
    assert(ob.countNodeNumbersInDirection == 4);

    ob.playerX = 1;
    ob.playerY = 9;
    ob.trajectory = 0;
    ob.direction = 0;
    ob.locateTrajectoryAndDirection(fp);
    ob.locateRelativeTrajectory();
    assert(ob.direction == SW);
    assert(ob.trajectory_right == 1);
    assert(ob.countNodeNumbersInDirection == 5);

    ob.playerX = 2;
    ob.playerY = 9;
    ob.trajectory = 0;
    ob.direction = 0;
    ob.locateTrajectoryAndDirection(fp);
    ob.locateRelativeTrajectory();
    assert(ob.direction == SW);
    assert(ob.trajectory_right == 1);
    assert(ob.countNodeNumbersInDirection == 5);

    ob.playerX = 3;
    ob.playerY = 8;
    ob.trajectory = 0;
    ob.direction = 0;
    ob.locateTrajectoryAndDirection(fp);
    ob.locateRelativeTrajectory();
    assert(ob.direction == W);
    assert(ob.trajectory_front == 1);
    assert(ob.countNodeNumbersInDirection == 6);

}

TEST(ObservationDirectionTrajectory4, BasicAssertions) {

    vector<vector<int>> grid;
    for (int i=0; i<GRID_SPAN; i++) {
        std::vector<int> row(GRID_SPAN, 0);
        grid.push_back(row);
    }

    const int TOTAL_FIXED_OBSTACLES = 2;
    int blockObstacles[TOTAL_FIXED_OBSTACLES][4] = {
            //x_s, x_e, y_s, y_e
            {1, 2, 5, 7},
            {3, 4, 3, 5}

    };

    // fill with static obstacles
    FixedObstacles fixedObstacles;
    for(int obstacle=0; obstacle<TOTAL_FIXED_OBSTACLES; obstacle++) {
        int x_s = blockObstacles[obstacle][0];
        int x_e = blockObstacles[obstacle][1];
        int y_s = blockObstacles[obstacle][2];
        int y_e = blockObstacles[obstacle][3];
        fixedObstacles.createBlockObstacle(x_s, x_e, y_s, y_e, grid);
    }

    auto fp = std::make_shared<findPath>(grid, 0, 6, 9, 5);
    assert (fp->findPathToDestination());
    observation ob;
    ob.playerX = 4;
    ob.playerY = 7;
    ob.locateTrajectoryAndDirection(fp);
    ob.locateRelativeTrajectory();
    assert(ob.direction == SW);
    assert(ob.trajectory_right == 1);

    ob.playerX = 5;
    ob.playerY = 6;
    ob.trajectory = 0;
    ob.direction = 0;
    ob.locateTrajectoryAndDirection(fp);
    ob.locateRelativeTrajectory();
    assert(ob.direction == S);
    assert(ob.trajectory_right == 1);

    ob.playerX = 3;
    ob.playerY = 6;
    ob.trajectory = 0;
    ob.direction = 0;
    ob.locateTrajectoryAndDirection(fp);
    ob.locateRelativeTrajectory();
    assert(ob.direction == SW);
    assert(ob.trajectory_left == 1);

    ob.playerX = 6;
    ob.playerY = 3;
    ob.trajectory = 0;
    ob.direction = 0;
    ob.locateTrajectoryAndDirection(fp);
    ob.locateRelativeTrajectory();
    assert(ob.direction == S);
    assert(ob.trajectory_left == 2);


}