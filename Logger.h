//
// Created by Debraj Ray on 2022-06-10.
//

#ifndef EXAMPLE_LOGGER_H
#define EXAMPLE_LOGGER_H

#include "gameConstants.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <memory>

using namespace std;
using namespace RTS;

class Logger {

    ofstream logfile;
    ofstream logfileObstacles;
    ofstream logfileEnemies;
    ofstream logfilePath;

    const string LOG_FILE = "/Users/debrajray/MyComputer/RL-A-STAR-THESIS/log/log.txt";
    const string LOG_FILE_OBSTACLES = "/Users/debrajray/MyComputer/RL-A-STAR-THESIS/log/log_obstacles.txt";
    const string LOG_FILE_ENEMIES = "/Users/debrajray/MyComputer/RL-A-STAR-THESIS/log/log_enemies.txt";
    const string LOG_FILE_PATH = "/Users/debrajray/MyComputer/RL-A-STAR-THESIS/log/log_path.txt";

    void printBoard_(const std::vector<std::vector<int>> &grid);

public:

    LOG_LEVEL level;

    static LOG_LEVEL GLOBAL_LOG_LEVEL;

    bool turnLogOff = false;

    Logger(LOG_LEVEL l) : level(l) {

    }

    void openLogFile();

    void closeLogFile();

    Logger* logDebug(string output);

    Logger* logInfo(string output);

    Logger* logDebug(double output);

    Logger* logInfo(double output);

    Logger* logToFileInfo(string output);

    Logger* logToFileInfo(double output);

    Logger* endLineInfoFile();

    Logger* endLineDebug();

    Logger* endLineInfo();

    Logger* printBoardDebug(std::vector<std::vector<int>> &grid);

    Logger* printBoardInfo(const std::vector<std::vector<int>> &grid);

    void logObstaclesToFile(float (&fov)[FOV_WIDTH][FOV_WIDTH]);

    void logEnemiesToFile(float (&fov)[FOV_WIDTH][FOV_WIDTH]);

    void logPathToFile(float (&fov)[FOV_WIDTH][FOV_WIDTH]);

};


#endif //EXAMPLE_LOGGER_H
