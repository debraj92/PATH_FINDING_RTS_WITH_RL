//
// Created by Debraj Ray on 2023-04-22.
//

#include "GameMaps.h"
#include "gameConstants.h"

void GameMaps::generateNextMap(vector<std::vector<int>> &grid, vector<enemy> &enemies, vector<int> &uimap) {

    const string fileName = "/Users/debrajray/MyComputer/658/project/warcraftmaps/AR0043SR.map";
    std::ifstream file(fileName);
    if (file.is_open()) {
        std::string line;
        /**
         * discard 4 lines
         * type, height, width, map
         */
        for(int i=0; i<4; ++i) {
            std::getline(file, line);
        }
        /**
         * Now we have to read 512 X 512 data
         */
        line = "";
        if(grid.capacity() == 0) {
            grid.resize(GRID_SPAN);
        }
        int row = 0, col = 0;
        while (std::getline(file, line)) {
            if(grid[row].capacity() == 0) {
                grid[row].resize(GRID_SPAN);
            }
            for(char x: line) {
                if(x == '.') {
                   grid[row][col] = 0;
                    uimap.emplace_back(1);
                } else {
                    grid[row][col] = -1;
                    uimap.emplace_back(10);
                }
                ++col;
            }
            ++row;
            col = 0;
        }
        file.close();
    } else {
        throw std::runtime_error("Could not open file: " + fileName);
    }
}
