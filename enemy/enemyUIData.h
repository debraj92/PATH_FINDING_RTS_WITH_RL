//
// Created by Debraj Ray on 2022-09-30.
//

#ifndef EXAMPLE_ENEMYUIDATA_H
#define EXAMPLE_ENEMYUIDATA_H

class enemyUIData {
public:
    int id;
    int x;
    int y;
    bool hasChangedOrientation;
    bool isOrientationLeft = true;
    bool isDead;
};

#endif //EXAMPLE_ENEMYUIDATA_H
