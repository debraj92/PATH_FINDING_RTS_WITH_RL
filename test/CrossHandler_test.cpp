//
// Created by Debraj Ray on 2022-11-12.
//

#include <gtest/gtest.h>
#include "../CrossMoveHandler.h"

TEST(CrossMoves, t1) {
    CrossMoveHandler c;
    c.addMove({1, 2, 2, 3, 1});
    c.addMove({2, 2, 1, 3, 2});
    c.addMove({0, 5, 1, 6, 3});
    c.addMove({0, 6, 1, 5, 4});
    c.addMove({4, 4, 5, 5, 5});
    c.addMove({5, 5, 4, 4, 6});

    c.addMove({4, 1, 4, 2, 7});
    c.addMove({5, 1, 5, 2, 8});
    c.addMove({2, 5, 3, 5, 9});
    c.addMove({3, 6, 2, 6, 10});
    c.addMove({0, 0, 0, 1, 11});
    c.addMove({0, 2, 0, 3, 12});

    c.addMove({2, 4, 2, 3, 13});
    c.addMove({3, 0, 3, 1, 14});
    c.addMove({3, 1, 3, 0, 15});
    c.addMove({6, 6, 5, 7, 16});
    c.addMove({6, 7, 5, 6, 17});
    c.addMove({5, 4, 4, 5, 18});


    c.checkForCrossMoves();
    auto units = c.getUnitsKilledInCrossMove();
    vector<int> corrects {1,2,3,4,5,6,14,15,16,17,18};
    for (int correct: corrects) {
        assert(units.count(correct));
    }
    assert(corrects.size() == units.size());
}

TEST(CrossMoves, t2) {
    CrossMoveHandler c;
    c.addMove({1, 2, 2, 3, 1});
    c.addMove({1, 3, 2, 2, 2});
    c.addMove({2, 4, 2, 3, 3});
    c.addMove({1, 7, 0, 8, 4});
    c.addMove({0, 8, 1, 7, 5});
    c.addMove({1, 8, 0, 7, 6});

    c.addMove({4, 6, 5, 6, 7});
    c.addMove({5, 6, 5, 7, 8});
    c.addMove({4, 7, 5, 7, 9});

    c.addMove({4, 2, 5, 1, 10});
    c.addMove({5, 1, 4, 2, 11});
    c.addMove({6, 0, 5, 1, 12});

    c.addMove({6, 3, 5, 4, 13});
    c.addMove({5, 3, 6, 4, 14});
    c.addMove({4, 3, 5, 4, 15});

    c.addMove({4, 9, 5, 9, 16});
    c.addMove({5, 9, 4, 9, 17});

    c.addMove({7, 7, 8, 8, 18});
    c.addMove({9, 8, 8, 9, 19});

    c.checkForCrossMoves();
    auto units = c.getUnitsKilledInCrossMove();
    vector<int> corrects {1,2,4,5,6,10,11,13,14,16,17};
    for (int correct: corrects) {
        assert(units.count(correct));
    }
    assert(corrects.size() == units.size());
}