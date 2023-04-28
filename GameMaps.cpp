//
// Created by Debraj Ray on 2023-04-22.
//

#include "GameMaps.h"
#include "gameConstants.h"

void GameMaps::generateNextMap(vector<std::vector<int>> &grid, vector<enemy> &enemies) {

    const string fileName = "/Users/debrajray/MyComputer/RL-A-STAR-THESIS/benchmark-maps/wc3maps512-map/divideandconquer.map";
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
                } else {
                    grid[row][col] = -1;
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

    populateEnemies(grid, enemies);
}

AbstractGraph GameMaps::createAbstractGraph(RealWorld &rw, vector<std::vector<int>> &grid) {
    return move(AbstractGraph(rw, 4));
}

void GameMaps::populateEnemies(vector<std::vector<int>> &grid, vector<enemy> &enemies) {
    auto rw = createRealWorld(grid);
    auto abG = createAbstractGraph(rw, grid);
    abG.createAbstractGraph();
    auto vec_abNodes = abG.getAllAbstractNodes();
    int enemy_id = 1;
    for(const auto& abNode: vec_abNodes) {
        auto x = abNode.centroidRealNode.first;
        auto y = abNode.centroidRealNode.second;
        if (grid[x][y] == 0) {
            enemies.emplace_back(grid, x, y, enemy_id);
            enemy_id = enemy_id + 1 == PLAYER_ID ? enemy_id + 2 : enemy_id + 1;
        }
    }
}

RealWorld GameMaps::createRealWorld(vector<std::vector<int>> &grid) {
    RealWorld rw;
    rw.loadMap(grid);
    return move(rw);
}
