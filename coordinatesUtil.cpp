//
// Created by Debraj Ray on 2022-01-04.
//

#include "coordinatesUtil.h"

    /**
     *
     *     --->y (columns)
     *     |
     *     v
     *     x (rows)
     */

/**
*    NW  N  NE
*       |
*     W- -E
*       |
*  SW  S  SE
*/


int coordinatesUtil::setNCoordinates(int &x, int &y) {
    if (x > 0) {
        if(verifyObstacle(x-1,y)) {
            x--;
            return 0;
        }
    }
    return -1;
}


int coordinatesUtil::setSWCoordinates(int& x, int& y) {
    if (y > 0 && x < GRID_SPAN_ - 1) {
        if(verifyObstacle(x+1,y-1)) {
            y--;
            x++;
            return 0;
        }
    }
    return -1;
}

int coordinatesUtil::setSECoordinates(int& x, int& y) {
    if (x < GRID_SPAN_ - 1 && y < GRID_SPAN_ - 1) {
        if(verifyObstacle(x+1,y+1)) {
            x++;
            y++;
            return 0;
        }
    }
    return -1;
}

int coordinatesUtil::setSCoordinates(int& x, int& y) {
    if (x < GRID_SPAN_ - 1) {
        if(verifyObstacle(x+1,y)) {
            x++;
            return 0;
        }
    }
    return -1;
}

int coordinatesUtil::setNECoordinates(int& x, int& y) {
    if (x > 0 && y < GRID_SPAN_ - 1) {
        if(verifyObstacle(x-1,y+1)) {
            y++;
            x--;
            return 0;
        }
    }
    return -1;
}

int coordinatesUtil::setNWCoordinates(int &x, int &y) {
    if (y > 0 && x > 0) {
        if(verifyObstacle(x-1,y-1)) {
            x--;
            y--;
            return 0;
        }
    }
    return -1;
}


int coordinatesUtil::setECoordinates(int& x, int& y) {
    if (y < GRID_SPAN_ - 1) {
        if(verifyObstacle(x,y+1)) {
            y++;
            return 0;
        }
    }
    return -1;
}

int coordinatesUtil::setWCoordinates(int& x, int& y) {
    if (y > 0) {
        if(verifyObstacle(x,y-1)) {
            y--;
            return 0;
        }
    }
    return -1;
}

int coordinatesUtil::setStraightActionCoordinates(int& x, int& y, int direction) {
    switch(direction) {
        case N:
            return setNCoordinates(x,y);
        case NE:
            return setNECoordinates(x,y);
        case NW:
            return setNWCoordinates(x,y);
        case E:
            return setECoordinates(x,y);
        case W:
            return setWCoordinates(x,y);
        case S:
            return setSCoordinates(x,y);
        case SE:
            return setSECoordinates(x,y);
        case SW:
            return setSWCoordinates(x,y);
        default:
            return -1;
    }
}

int coordinatesUtil::setDodgeDiagonalLeftActionCoordinates(int &x, int &y, int direction) {
    switch(direction) {
        case N:
            return setNWCoordinates(x,y);
        case NE:
            return setNCoordinates(x,y);
        case NW:
            return setWCoordinates(x,y);
        case E:
            return setNECoordinates(x,y);
        case W:
            return setSWCoordinates(x,y);
        case S:
            return setSECoordinates(x,y);
        case SE:
            return setECoordinates(x,y);
        case SW:
            return setSCoordinates(x,y);
        default:
            return -1;
    }
}

int coordinatesUtil::setDodgeLeftActionCoordinates(int &x, int &y, int direction) {
    switch(direction) {
        case N:
            return setWCoordinates(x,y);
        case NE:
            return setNWCoordinates(x,y);
        case NW:
            return setSWCoordinates(x,y);
        case E:
            return setNCoordinates(x,y);
        case W:
            return setSCoordinates(x,y);
        case S:
            return setECoordinates(x,y);
        case SE:
            return setNECoordinates(x,y);
        case SW:
            return setSECoordinates(x,y);
        default:
            return -1;

    }
}

int coordinatesUtil::setDodgeRightActionCoordinates(int &x, int &y, int direction) {
    switch(direction) {
        case N:
            return setECoordinates(x,y);
        case NE:
            return setSECoordinates(x,y);
        case NW:
            return setNECoordinates(x,y);
        case E:
            return setSCoordinates(x,y);
        case W:
            return setNCoordinates(x,y);
        case S:
            return setWCoordinates(x,y);
        case SE:
            return setSWCoordinates(x,y);
        case SW:
            return setNWCoordinates(x,y);
        default:
            return -1;
    }
}

int coordinatesUtil::setDodgeDiagonalRightActionCoordinates(int &x, int &y, int direction) {
    switch(direction) {
        case N:
            return setNECoordinates(x,y);
        case NE:
            return setECoordinates(x,y);
        case NW:
            return setNCoordinates(x,y);
        case E:
            return setSECoordinates(x,y);
        case W:
            return setNWCoordinates(x,y);
        case S:
            return setSWCoordinates(x,y);
        case SE:
            return setSCoordinates(x,y);
        case SW:
            return setWCoordinates(x,y);
        default:
            return -1;
    }
}

/// True if no obstacle
bool coordinatesUtil::verifyObstacle(int x, int y) {
    return grid[x][y] >= 0;
}

