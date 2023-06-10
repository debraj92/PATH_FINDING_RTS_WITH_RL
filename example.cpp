#include <iostream>
#include <fstream>
#include "gameConstants.h"
#include "Logger.h"
#include "player.h"
#include "trainingMaps.h"
#include <sys/resource.h>
#include "GameController.h"
#include "GameMaps.h"

#include <chrono>

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

void increaseStackSize() {
    const rlim_t kStackSize = 16 * 1024 * 1024;   // min stack size = 16 MB
    struct rlimit rl;
    int result;

    result = getrlimit(RLIMIT_STACK, &rl);
    if (result == 0)
    {
        if (rl.rlim_cur < kStackSize)
        {
            rl.rlim_cur = kStackSize;
            result = setrlimit(RLIMIT_STACK, &rl);
            if (result != 0)
            {
                fprintf(stderr, "setrlimit returned result = %d\n", result);
            }
        }
    }
}

void runTesting(player &player1) {
    TestResult t{};
    int sx=0;
    int sy=0;
    int dx=0;
    int dy=0;
    int totalDamage = 0;
    double totalPathRatio = 0;
    int maxMemoryUsed = 0;
    trainingMaps tm(true);
    float countDestinationReach = 0;
    float death = 0;
    float max_ep = 1000;
    //player1.enableInfiniteLife();
    auto t1 = high_resolution_clock::now();
    for (int i=1; i<= max_ep; i++) {
        cout<<"Episode: "<<i<<endl;
        vector<vector<int>> grid;
        for (int i=0; i<GRID_SPAN; i++) {
            std::vector<int> row(GRID_SPAN, 0);
            grid.push_back(row);
        }
        std::vector<enemy> enemies;
        tm.generateNextMap(grid, enemies);

        tm.setSourceAndDestination(grid, sx, sy, dx, dy);
        //tm.setSourceAndDestinationFixed(sx, sy, dx, dy);
        cout<<"("<<sx<<","<<sy<<") -> ("<<dx<<","<<dy<<")"<<endl;

        player1.playGame(grid, enemies, sx, sy, dx, dy, t);


        if(t.final_x == t.destination_x and t.final_y == t.destination_y) {
            countDestinationReach++;
        }

        if(player1.life_left == 0) {
            death++;
        }

        totalDamage += t.damage;
        totalPathRatio += t.pathRatio;
        maxMemoryUsed += t.maxMemoryUsed;
    }

    auto t2 = high_resolution_clock::now();
    duration<double, std::milli> ms_double = t2 - t1;
    double averageExecTime = ms_double.count() / (double)max_ep;
    double averageDamage = (double)totalDamage / (double)max_ep;
    double averagePathRatio = (double)totalPathRatio / (double)max_ep;
    double averageMaxMemoryUsed = (double)maxMemoryUsed / (double)max_ep;

    cout<<"Average execution time "<<averageExecTime<<endl;
    cout<<"Average damage "<< averageDamage<<endl;
    cout<<"Average path ratio "<< averagePathRatio<<endl;
    cout<<"Average max memory used "<< averageMaxMemoryUsed<<endl;


    float reach_percent = countDestinationReach * 100 / max_ep;
    cout<<"% reach "<<reach_percent<<endl;


    float death_percent = death * 100 / max_ep;
    cout<<"% death "<<death_percent<<endl;


}

void generateMaps() {
    long randomNumber = std::chrono::system_clock::now().time_since_epoch().count();
    trainingMaps tm(false);
    tm.serializeRandomMap("map1", randomNumber);
    cout<<endl;
    tm.serializeRandomMap("map2", randomNumber * 7);
    cout<<endl;
    tm.serializeRandomMap("map3", randomNumber * 13);
    cout<<endl;
    tm.serializeRandomMap("map4", randomNumber * 5);
    cout<<endl;
    tm.serializeRandomMap("map5", randomNumber * 3);
    cout<<endl;
    tm.serializeRandomMap("map6", randomNumber * 23);
    cout<<endl;
    tm.serializeRandomMap("map7", randomNumber * 19);
    cout<<endl;
    tm.serializeRandomMap("map8", randomNumber * 17);
    cout<<endl;
    tm.serializeRandomMap("map9", randomNumber * 101);
    cout<<endl;
    tm.serializeRandomMap("map10", randomNumber * 103);
    cout<<endl;
    tm.serializeRandomMap("map11", randomNumber * 107);
    cout<<endl;
    tm.serializeRandomMap("map12", randomNumber * 113);
    cout<<endl;
    tm.serializeRandomMap("map13", randomNumber * 127);
    cout<<endl;
    tm.serializeRandomMap("map14", randomNumber * 131);
    cout<<endl;
    tm.serializeRandomMap("map15", randomNumber * 139);
    cout<<endl;
    tm.serializeRandomMap("map16", randomNumber * 139);
    cout<<endl;
}


void runOnGameMaps() {

    vector<vector<int>> grid;
    for (int i=0; i<GRID_SPAN; i++) {
        std::vector<int> row(GRID_SPAN, 0);
        grid.push_back(row);
    }
    vector<GameMaps::src_dst_data> startEnds;
    std::vector<enemy> enemiesOriginal;
    std::vector<enemy> enemies;
    GameMaps gm;
    gm.generateNextMap(grid);
    gm.populateSourceDestinations(startEnds);
    gm.populateEnemies(grid, enemiesOriginal);
    vector<TestResult> results;
    while (!gm.isEndOfSrcDst()) {
        /**
         * Set Up New Player
         * Enable Neural Net if RL is enabled
         */
        player pl(false, false);
        pl.enableInfiniteLife();

        pl.enableBaseLinePlayer();
        //pl.enablePotentialFieldPlayer();

        /**
         * Start New Episode
         */
        auto data = gm.generateNextSourceAndDestination(startEnds);
        enemies.clear();
        std::copy(enemiesOriginal.begin(), enemiesOriginal.end(), back_inserter(enemies));
        TestResult t{};
        auto t1 = high_resolution_clock::now();
        pl.playGame(grid, enemies, data.startX, data.startY, data.endX, data.endY, t);
        auto t2 = high_resolution_clock::now();
        duration<double, std::milli> ms_double = t2 - t1;
        t.executionTime = ms_double.count();
        t.pathLength = data.pathLength;
        results.push_back(t);
        cout<<"D: "<<t.damage<<" PR: "<<t.pathRatio<<" M: "<<t.maxMemoryUsed<<" T: "<<t.executionTime<<" R: "<<t.reached<<" L "<< t.pathLength <<endl;
    }

    ofstream file;
    //string output = "/Users/debrajray/MyComputer/RL-A-STAR-THESIS/benchmark-maps/results/thecrucible_rl.csv";
    string output = "/Users/debrajray/MyComputer/RL-A-STAR-THESIS/benchmark-maps/results/thecrucible_pra*.csv";
    //string output = "/Users/debrajray/MyComputer/RL-A-STAR-THESIS/benchmark-maps/results/thecrucible_pf.csv";
    file.open(output);
    for(auto &t: results) {
        file << t.damage << "," << t.pathRatio << "," << t.maxMemoryUsed << "," << t.executionTime << "," << t.reached << "," << t.pathLength << endl;
    }
    file.close();
}

void generateEnemiesForWarcraftMap() {
    vector<vector<int>> grid;
    for (int i=0; i<GRID_SPAN; i++) {
        std::vector<int> row(GRID_SPAN, 0);
        grid.push_back(row);
    }
    std::vector<enemy> enemies;
    GameMaps gm;
    gm.generateNextMap(grid);
    gm.serializeEnemies(grid, enemies);
}

void generateStartAndEndCoordinatesWM() {
    GameMaps gm;
    gm.serializeStartAndEndPoints();
}

int main() {

    increaseStackSize();

    using namespace RTS;
    Logger::GLOBAL_LOG_LEVEL = LOG_LEVEL::INFO;

    //player player1(true);
    //player1.learnGame();

    /*
    player player1(false);
    /// Enable baseline for comparison
    //player1.enableBaseLinePlayer();
    player1.enablePotentialFieldPlayer();
    //runTesting(player1);
     */
    //runOnGameMaps();

    //generateMaps();

    /// Enable to generate enemies for warcraft maps
    generateEnemiesForWarcraftMap();
    //generateStartAndEndCoordinatesWM();


    /// UI works only for training-maps1

    /**
     * For Warcraft Maps,
     * set GRID_SPAN = 28,
     * set ABSTRACT_SECTOR_PRA_STAR = 7 or 4
     * set useDefaultMap to false
     * Update GameMaps to pick the correct maps
     *
     * For Default Maps,
     * set GRID_SPAN = 27
     * set ABSTRACT_SECTOR_PRA_STAR = 9 or 3
     * set useDefaultMap to true
     *
     */

    GameController control;
    control.startGame(false);
    //control.startGame(true);

    return 0;
}
