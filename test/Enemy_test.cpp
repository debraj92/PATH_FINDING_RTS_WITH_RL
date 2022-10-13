//
// Created by Debraj Ray on 2022-07-12.
//

#include <gtest/gtest.h>
#include "../enemy/enemy.h"

void Enemy_createEmptyGrid(vector<std::vector<int>> &grid) {
    for (int i=0; i<GRID_SPAN; i++) {
        std::vector<int> row(GRID_SPAN, 0);
        grid.push_back(row);
    }
}

TEST(MovingEnemy, t1) {

    Logger logger(RTS::DEBUG);
    vector <std::vector<int>> grid;
    Enemy_createEmptyGrid(grid);
    enemy e1 = enemy(grid, 2, 2, 1);
    int t = 1;
    e1.lastKnownPlayerX = 5;
    e1.lastKnownPlayerY = 5;
    e1.doNextMove(t, grid, {4, 4, 0});
    assert(e1.current_x == 3 and e1.current_y == 3);

    enemy e2 = enemy(grid, 2, 2, 1);
    e2.lastKnownPlayerX = 7;
    e2.lastKnownPlayerY = 1;
    e2.doNextMove(t, grid, {7, 0, 0});
    assert(e2.current_x == 2 and e2.current_y == 2);


    enemy e3 = enemy(grid, 2, 2, 1);
    e3.lastKnownPlayerX = 3;
    e3.lastKnownPlayerY = 1;
    e3.doNextMove(t++, grid, {3, 0, 0});
    assert(e3.current_x == 3 and e3.current_y == 1);
    e3.doNextMove(t, grid, {3, 0, 0});
    assert(e3.current_x == 3 and e3.current_y == 0);
}

TEST(MovingEnemy, t2) {
    vector <std::vector<int>> grid;
    Enemy_createEmptyGrid(grid);
    enemy e1 = enemy(grid, 2, 2, 1);
    e1.max_moves = 100;
    int t=1;
    for(int i= 1; i<=50; i++) {
        e1.lastKnownPlayerX = 2;
        e1.lastKnownPlayerY = 4;
        e1.current_x = 2;
        e1.current_y = 2;
        e1.doNextMove(t, grid, {2, 3, 0});
        cout<<e1.current_x<<","<<e1.current_y<<endl;
    }
}

TEST(MovingEnemy, t3) {
    vector <std::vector<int>> grid;
    Enemy_createEmptyGrid(grid);
    enemy e1 = enemy(grid, 0, 2, 1);
    e1.max_moves = 100;
    int t=1;
    for(int i= 1; i<=50; i++) {
        e1.lastKnownPlayerX = 0;
        e1.lastKnownPlayerY = 4;
        e1.current_x = 0;
        e1.current_y = 2;
        e1.doNextMove(t, grid, {0, 3, 0});
        assert(e1.current_x == 0 and e1.current_y == 3);
    }
}

TEST(MovingEnemy, t4) {
    vector <std::vector<int>> grid;
    Enemy_createEmptyGrid(grid);
    grid[1][2] =-1;
    enemy e1 = enemy(grid, 0, 2, 1);
    e1.max_moves = 100;
    int t=1;
    for(int i= 1; i<=50; i++) {
        e1.lastKnownPlayerX = 0;
        e1.lastKnownPlayerY = 4;
        e1.current_x = 0;
        e1.current_y = 2;
        e1.doNextMove(t, grid, {0, 3, 0});
        assert(e1.current_x == 0 and e1.current_y == 3);
    }
}