//
// Created by Debraj Ray on 2022-07-08.
//

#include <gtest/gtest.h>
#include <vector>
#include "../AStar_.h"
#include "../gameConstants.h"
#include "../fixedobstacles.h"
#include "../enemy/enemy.h"
#include "../player.h"
#include "../objectLocator.h"

using namespace std;

void AStarChase_createAllFixedObstacles(int TOTAL_FIXED_OBSTACLES, int blockObstacles[][4], vector<std::vector<int>> &grid) {

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

void AStarChase_createGrid1(vector<std::vector<int>> &grid) {
    for (int i=0; i<GRID_SPAN; i++) {
        std::vector<int> row(GRID_SPAN, 0);
        grid.push_back(row);
    }
}

void AStarChase_createGrid2(vector<std::vector<int>> &grid) {
    for (int i=0; i<GRID_SPAN; i++) {
        std::vector<int> row(GRID_SPAN, 0);
        grid.push_back(row);
    }

    const int TOTAL_FIXED_OBSTACLES = 5;
    int blockObstacles[TOTAL_FIXED_OBSTACLES][4] = {
            //x_s, x_e, y_s, y_e
            {1, 2, 2, 2},
            {2, 3, 3, 3},
            {5, 7, 1, 1},
            {7, 7, 1, 3},
            {4, 6, 7, 7}
    };

    AStarChase_createAllFixedObstacles(TOTAL_FIXED_OBSTACLES, blockObstacles, grid);
}


TEST(AStarChase, t1) {

    vector<std::vector<int>> grid;
    AStarChase_createGrid1(grid);
    Logger logger(LOG_LEVEL::DEBUG);

    player p(false);
    p.current_x = 0;
    p.current_y = 7;
    int dst_x = GRID_SPAN - 1;
    int dst_y = 0;
    p.life_left = 10;
    enemy e1 = enemy(grid, 4, 7, 1);
    grid[p.current_x][p.current_y] = 9;
    grid[e1.current_x][e1.current_y] = e1.id;

    auto fp = findPath(grid, p.current_x, p.current_y, dst_x, dst_y);
    fp.findPathToDestination();
    logger.printBoardDebug(grid);
    int t = 1;
    do {

        grid[p.current_x][p.current_y] = 0;
        fp.isOnTrack(p.current_x, p.current_y);
        fp.calculateNextPosition();
        int playerSavedX = p.current_x;
        int playerSavedY = p.current_y;
        p.current_x = fp.getNext_x();
        p.current_y = fp.getNext_y();
        grid[p.current_x][p.current_y] = 9;
        // dont follow if player has reached
        if (p.current_x == dst_x and p.current_y == dst_y) {
            logger.printBoardDebug(grid);
            cout<<"Player Reached"<<endl;
            break;
        }
        // player stepped on enemy - death
        if (e1.current_x == p.current_x and e1.current_y == p.current_y) {
            cout<<"Player dead"<<endl;
            p.takeDamage(e1.getAttackPoints());
            break;
        } else {
            e1.doNextMove(t++, grid, {playerSavedX, playerSavedY, 0});
            logger.printBoardDebug(grid);
            // move enemy and check for kill
            if (e1.current_x == p.current_x and e1.current_y == p.current_y) {
                cout<<"Player dead"<<endl;
                p.takeDamage(e1.getAttackPoints());
                break;
            }
        }
    } while (p.current_x != dst_x or p.current_y != dst_y);

    assert(p.life_left > 0);

}

TEST(AStarChase, t2) {

    vector<std::vector<int>> grid;
    AStarChase_createGrid1(grid);
    Logger logger(LOG_LEVEL::DEBUG);

    player p(false);
    p.current_x = 0;
    p.current_y = 7;
    int dst_x = GRID_SPAN - 1;
    int dst_y = 0;
    p.life_left = 10;
    enemy e1 = enemy(grid, 4, 2, 1);
    grid[p.current_x][p.current_y] = 9;
    grid[e1.current_x][e1.current_y] = e1.id;

    auto fp = findPath(grid, p.current_x, p.current_y, dst_x, dst_y);
    fp.findPathToDestination();
    logger.printBoardDebug(grid);
    int t = 1;
    do {

        grid[p.current_x][p.current_y] = 0;
        fp.isOnTrack(p.current_x, p.current_y);
        fp.calculateNextPosition();
        int playerSavedX = p.current_x;
        int playerSavedY = p.current_y;
        p.current_x = fp.getNext_x();
        p.current_y = fp.getNext_y();
        grid[p.current_x][p.current_y] = 9;
        // dont follow if player has reached
        if (p.current_x == dst_x and p.current_y == dst_y) {
            logger.printBoardDebug(grid);
            cout<<"Player Reached"<<endl;
            break;
        }
        // player stepped on enemy - death
        if (e1.current_x == p.current_x and e1.current_y == p.current_y) {
            cout<<"Player dead"<<endl;
            p.takeDamage(e1.getAttackPoints());
            break;
        } else {
            e1.doNextMove(t++, grid, {playerSavedX, playerSavedY, 0});
            logger.printBoardDebug(grid);
            // move enemy and check for kill
            if (e1.current_x == p.current_x and e1.current_y == p.current_y) {
                cout<<"Player dead"<<endl;
                p.takeDamage(e1.getAttackPoints());
                break;
            }
        }
    } while (p.current_x != dst_x or p.current_y != dst_y);

    assert(p.life_left > 0);

}

TEST(AStarChase, t4) {

    vector<std::vector<int>> grid;
    AStarChase_createGrid2(grid);
    Logger logger(LOG_LEVEL::DEBUG);

    player p(false);
    p.current_x = GRID_SPAN - 1;
    p.current_y = 4;
    int dst_x = 0;
    int dst_y = 4;
    p.life_left = 10;
    enemy e1 = enemy(grid, 6, 8, 1);
    e1.enemyVisionRadius = VISION_RADIUS + 2;
    grid[p.current_x][p.current_y] = 9;
    grid[e1.current_x][e1.current_y] = e1.id;

    auto fp = findPath(grid, p.current_x, p.current_y, dst_x, dst_y);
    fp.findPathToDestination();
    logger.printBoardDebug(grid);
    int t=1;
    do {

        grid[p.current_x][p.current_y] = 0;
        fp.isOnTrack(p.current_x, p.current_y);
        fp.calculateNextPosition();
        int playerSavedX = p.current_x;
        int playerSavedY = p.current_y;
        p.current_x = fp.getNext_x();
        p.current_y = fp.getNext_y();
        grid[p.current_x][p.current_y] = 9;
        // dont follow if player has reached
        if (p.current_x == dst_x and p.current_y == dst_y) {
            logger.printBoardDebug(grid);
            cout<<"Player Reached"<<endl;
            break;
        }
        // player stepped on enemy - death
        if (e1.current_x == p.current_x and e1.current_y == p.current_y) {
            cout<<"Player dead"<<endl;
            p.takeDamage(e1.getAttackPoints());
            break;
        } else {
            e1.doNextMove(t++, grid, {playerSavedX, playerSavedY, 0});
            logger.printBoardDebug(grid);
            // move enemy and check for kill
            if (e1.current_x == p.current_x and e1.current_y == p.current_y) {
                cout<<"Player dead"<<endl;
                p.takeDamage(e1.getAttackPoints());
                break;
            }
        }
    } while (p.current_x != dst_x or p.current_y != dst_y);

    // passes only for A* chase
    assert(p.life_left == 0);

}

TEST(AStarChase, t3) {

    vector<std::vector<int>> grid;
    AStarChase_createGrid2(grid);
    Logger logger(LOG_LEVEL::DEBUG);

    player p(false);
    p.current_x = 0;
    p.current_y = 5;
    int dst_x = GRID_SPAN - 1;
    int dst_y = 5;
    p.life_left = 10;
    enemy e1 = enemy(grid, 4, 7, 1);
    grid[p.current_x][p.current_y] = 9;
    grid[e1.current_x][e1.current_y] = e1.id;

    auto fp = findPath(grid, p.current_x, p.current_y, dst_x, dst_y);
    fp.findPathToDestination();
    logger.printBoardDebug(grid);
    int t=1;
    do {

        grid[p.current_x][p.current_y] = 0;
        fp.isOnTrack(p.current_x, p.current_y);
        fp.calculateNextPosition();
        int playerSavedX = p.current_x;
        int playerSavedY = p.current_y;
        p.current_x = fp.getNext_x();
        p.current_y = fp.getNext_y();
        grid[p.current_x][p.current_y] = 9;
        // dont follow if player has reached
        if (p.current_x == dst_x and p.current_y == dst_y) {
            logger.printBoardDebug(grid);
            cout<<"Player Reached"<<endl;
            break;
        }
        // player stepped on enemy - death
        if (e1.current_x == p.current_x and e1.current_y == p.current_y) {
            cout<<"Player dead"<<endl;
            p.takeDamage(e1.getAttackPoints());
            break;
        } else {
            e1.doNextMove(t++, grid, {playerSavedX, playerSavedY, 0});
            logger.printBoardDebug(grid);
            // move enemy and check for kill
            if (e1.current_x == p.current_x and e1.current_y == p.current_y) {
                cout<<"Player dead"<<endl;
                p.takeDamage(e1.getAttackPoints());
                break;
            }
        }
    } while (p.current_x != dst_x or p.current_y != dst_y);

    assert(p.life_left == 0);

}
