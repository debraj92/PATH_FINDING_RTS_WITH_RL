//
// Created by Debraj Ray on 2022-06-10.
//

#include <sstream>
#include <iomanip>
#include "Logger.h"
#include <vector>
#include <cstdio>

using namespace RTS;
using namespace std;

LOG_LEVEL Logger::GLOBAL_LOG_LEVEL = INFO;

Logger* Logger::logDebug(string output) {
    if (turnLogOff) {
        return this;
    }
    switch (GLOBAL_LOG_LEVEL) {
        case DEBUG:
            if (level != OFF) {
                cout<<output;
            }
            return this;
        case INFO:
            switch (level) {
                case DEBUG:
                    cout<<output;
                    return this;
                case INFO:
                case OFF:
                    return this;
            }
            return this;
        case OFF:
            return this;
    }
}

Logger* Logger::logInfo(string output) {
    if (turnLogOff) {
        return this;
    }
    switch (GLOBAL_LOG_LEVEL) {
        case DEBUG:
            if (level != OFF) {
                cout<<output;
            }
            return this;
        case INFO:
            switch (level) {
                case DEBUG:
                case INFO:
                    cout<<output;
                    return this;
                case OFF:
                    return this;
            }
            return this;
        case OFF:
            return this;
    }
}

Logger* Logger::logDebug(double output) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << output;
    return logDebug(ss.str());
}

Logger* Logger::logInfo(double output) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << output;
    return logInfo(ss.str());
}

Logger *Logger::endLineDebug() {
    return logDebug("\n");
}

Logger *Logger::endLineInfo() {
    return logInfo("\n");
}

void Logger::printBoard_(const std::vector<std::vector<int>> &grid) {
    cout<<"print board"<<endl;
    for (int row=0; row<GRID_SPAN; row++) {
        for (int col=0; col<GRID_SPAN; col++) {
            cout<<grid[row][col];
            if(grid[row][col] <= -10) cout<<" ";
            else if (grid[row][col] >= 100) cout<<" ";
            else if(grid[row][col] < 0 or grid[row][col] >= 10) cout<<"  ";
            else cout<<"   ";
        }
        cout<<"\n";
    }
}

Logger *Logger::printBoardDebug(vector<std::vector<int>> &grid) {
    switch (GLOBAL_LOG_LEVEL) {
        case DEBUG:
            if (level != OFF) {
                printBoard_(grid);
            }
            return this;
        case INFO:
            switch (level) {
                case DEBUG:
                    printBoard_(grid);
                    return this;
                case INFO:
                case OFF:
                    return this;
            }
            return this;
        case OFF:
            return this;
    }
}

Logger *Logger::printBoardInfo(const vector<std::vector<int>> &grid) {
    switch (GLOBAL_LOG_LEVEL) {
        case DEBUG:
            if (level != OFF) {
                printBoard_(grid);
            }
            return this;
        case INFO:
            switch (level) {
                case DEBUG:
                case INFO:
                    printBoard_(grid);
                    return this;
                case OFF:
                    return this;
            }
            return this;
        case OFF:
            return this;
    }
}

Logger *Logger::logToFileInfo(string output) {
    switch (GLOBAL_LOG_LEVEL) {
        case DEBUG:
            if (level != OFF) {
                logfile<<output;
                std::flush(logfile);
            }
            return this;
        case INFO:
            switch (level) {
                case DEBUG:
                case INFO:
                    logfile<<output;
                    std::flush(logfile);
                    return this;
                case OFF:
                    return this;
            }
            return this;
        case OFF:
            return this;
    }
}

Logger *Logger::logToFileInfo(double output) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << output;
    return logToFileInfo(ss.str());
}

Logger *Logger::endLineInfoFile() {
    return logToFileInfo("\n");
}

void Logger::openLogFile() {
    logfile.open(LOG_FILE);
    logfileObstacles.open(LOG_FILE_OBSTACLES);
    logfileEnemies.open(LOG_FILE_ENEMIES);
    logfilePath.open(LOG_FILE_PATH);
}

void Logger::closeLogFile() {
    logfile.close();
    logfileObstacles.close();
    logfileEnemies.close();
    logfilePath.close();
}

void Logger::logObstaclesToFile(float (&fov)[FOV_WIDTH][FOV_WIDTH]) {
    for (int row=0; row<FOV_WIDTH; row++) {
        for (int col=0; col<FOV_WIDTH; col++) {
            logfileObstacles<<fov[row][col]<<" ";
        }
        logfileObstacles<<"\n";
    }
    logfileObstacles<<"\n";
    std::flush(logfileObstacles);
}

void Logger::logEnemiesToFile(float (&fov)[FOV_WIDTH][FOV_WIDTH]) {
    for (int row=0; row<FOV_WIDTH; row++) {
        for (int col=0; col<FOV_WIDTH; col++) {
            logfileEnemies<<fov[row][col]<<" ";
        }
        logfileEnemies<<"\n";
    }
    logfileEnemies<<"\n";
    std::flush(logfileEnemies);
}

void Logger::logPathToFile(float (&fov)[FOV_WIDTH][FOV_WIDTH]) {
    for (int row=0; row<FOV_WIDTH; row++) {
        for (int col=0; col<FOV_WIDTH; col++) {
            logfilePath<<fov[row][col]<<" ";
        }
        logfilePath<<"\n";
    }
    logfilePath<<"\n";
    std::flush(logfilePath);
}

