//
// Created by Debraj Ray on 2022-05-27.
//

#include <gtest/gtest.h>
#include <vector>
#include "../AStar_.h"
#include "../gameConstants.h"
#include "../fixedobstacles.h"
#include "../TestResult.h"

TEST(SingleAgent_t1, BasicAssertions) {
/*
    GameEnv gameEnv(true);
    gameEnv.createMap2();
    gameEnv.train();

    std::vector<enemy> enemies;
    fixedEnemy f1(4, 7, 1);
    fixedEnemy f2(4, 1, 2);
    fixedEnemy f3(5, 3, 3);
    fixedEnemy f4(4, 4, 4);
    fixedEnemy f5(1, 6, 5);
    enemies.push_back(f1);
    enemies.push_back(f2);
    enemies.push_back(f3);
    enemies.push_back(f4);
    enemies.push_back(f5);

    cout<<"****************************"<<endl;
    TestResult player = gameEnv.test(0, 0, GRID_SPAN - 1, GRID_SPAN - 1, enemies);
    ASSERT_TRUE(player.final_x == player.destination_x && player.final_y == player.destination_y);
    ASSERT_TRUE(player.total_rewards > 0);

    cout<<"****************************"<<endl;
    player = gameEnv.test(GRID_SPAN - 1, 0, (GRID_SPAN - 1)/2, (GRID_SPAN - 1), enemies);
    ASSERT_TRUE(player.final_x == player.destination_x && player.final_y == player.destination_y);
    ASSERT_TRUE(player.total_rewards > 0);

    cout<<"****************************"<<endl;
    player = gameEnv.test((GRID_SPAN - 1)/2, 0, (GRID_SPAN - 1)/2, GRID_SPAN - 1, enemies);
    ASSERT_TRUE(player.final_x == player.destination_x && player.final_y == player.destination_y);
    ASSERT_TRUE(player.total_rewards > 0);
*/
}

TEST(SingleAgent_t2, BasicAssertions) {
/*
    GameEnv gameEnv(true);
    gameEnv.createMap4();
    gameEnv.train();


    cout<<"****************************"<<endl;
    TestResult player = gameEnv.test(GRID_SPAN - 1, 0, GRID_SPAN/2, GRID_SPAN/2, gameEnv.enemies);
    ASSERT_TRUE(player.final_x == player.destination_x && player.final_y == player.destination_y);
    ASSERT_TRUE(player.total_rewards > 0);


    cout<<"****************************"<<endl;
    player = gameEnv.test(0, 0, (GRID_SPAN - 1), (GRID_SPAN - 1), gameEnv.enemies);
    ASSERT_TRUE(player.final_x == player.destination_x && player.final_y == player.destination_y);
    ASSERT_TRUE(player.total_rewards > 0);

    cout<<"****************************"<<endl;
    player = gameEnv.test((GRID_SPAN - 1)/2, 0, (GRID_SPAN)/2, (GRID_SPAN)/2, gameEnv.enemies);
    ASSERT_TRUE(player.final_x == player.destination_x && player.final_y == player.destination_y);
    ASSERT_TRUE(player.total_rewards > 0);
*/

}