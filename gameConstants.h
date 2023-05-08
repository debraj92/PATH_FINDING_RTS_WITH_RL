//
// Created by Debraj Ray on 2021-11-14.
//

#ifndef EXAMPLE_GAMECONSTANTS_H
#define EXAMPLE_GAMECONSTANTS_H

//const int MAX_EPISODES = 10000; // 20000;
//const int SESSION_TIMEOUT = 50; // 80;
//const int GRID_SPAN=6;//12;

/**
 * Debug Params
 */
const int MAX_EPISODES = 300000;   /// Must be greater than 8

const int GRID_SPAN = 28;//512; /// >= 10 when running unit tests [7 for gameSimulation test]
const int SESSION_TIMEOUT = GRID_SPAN * 5;

const int MIN_EXPLORATION_BEFORE_TRAINING = 4;

static const int MAX_STATES = 40000;

/**
 *    NW  N  NE
 *       |
 *     W- -E
 *       |
 *  SW  S  SE
 */

const int N=1;
const int NW=2;
const int W=3;
const int SW=4;
const int S=5;
const int SE=6;
const int E=7;
const int NE=8;

const int VISION_RADIUS = 4;
const int FOV_WIDTH = 2 * VISION_RADIUS + 1;

const int ENEMY_VISION_RADIUS = 3;
const int ENEMY_MAX_MOVES = 20;
const int MAX_ENEMY_DISTANCE_FOR_PURSUIT = 6;

const int GOAL_RADIUS = 1;

const int on_track = 10;
const int lower_bound_one_deviation = 11;
const int upper_bound_one_deviation = 14;

const int one_deviation_N = 11;
const int one_deviation_S = 12;
const int one_deviation_W = 13;
const int one_deviation_E = 14;

const int lower_bound_two_deviation = 21;
const int upper_bound_two_deviation = 24;

const int two_deviation_N = 21;
const int two_deviation_S = 22;
const int two_deviation_W = 23;
const int two_deviation_E = 24;

const int lower_bound_three_deviation = 31;
const int upper_bound_three_deviation = 34;

const int lower_bound_four_deviation = 41;
const int upper_bound_four_deviation = 44;


/// ACTIONS
const int ACTION_DODGE_LEFT = 0;
const int ACTION_DODGE_DIAGONAL_LEFT = 1;
const int ACTION_STRAIGHT = 2;
const int ACTION_DODGE_DIAGONAL_RIGHT = 3;
const int ACTION_DODGE_RIGHT = 4;

const int ACTION_SPACE = 5;

const int ACTION_SWITCH = 7; // deprecated
const int ACTION_REROUTE = 5; // deprecated
const int ACTION_FOLLOW = 8; // deprecated


/// REWARDS
const float REWARD_FACTOR = 5;
const float REWARD_REACH =  3 * REWARD_FACTOR;
const float REWARD_ACTION_UNAVAILABLE = -2 * REWARD_FACTOR;
const float REWARD_ACTION_LR = -1.5 * REWARD_FACTOR;
const float REWARD_TRACK_ONE_DIV = -0.2 * REWARD_FACTOR;
const float REWARD_TRACK_TWO_DIV = -0.4 * REWARD_FACTOR;
const float REWARD_TRACK_THREE_DIV = -0.7 * REWARD_FACTOR;
const float REWARD_TRACK_FOUR_DIV = -0.7 * REWARD_FACTOR;
const float REWARD_DEATH = -9 * REWARD_FACTOR;
const float REWARD_OFFTRACK = -6 * REWARD_FACTOR;


const int MAX_LIFE = 10;

const int MAX_DISTANCE = 100;

const int MAX_ABSTRACT_OBSERVATIONS = 50;

const int MAX_CAPACITY_REPLAY_BUFFER = 15000;

const int MIN_BUFFERED_EXPERIENCE_FOR_LEARNING = 1000;

const int SWITCH_TO_EXPLOITATION_ONLY_PERCENT = 95;

const int MAX_RESUME = GRID_SPAN * 3; // Approx average path size

const double MIN_EXPLOITATION_WINDOW_START_FOR_MEMORY = 0.5;

const int EXPLOITATION_START_PERCENT = 50;

const int MAX_REWARD_POINTS_IN_PLOT = 500;

const int PLAYER_ID = 9;

const int MAX_CHANNELS_CNN = 3; // obstacle, enemies, path

const double Q_REROUTE_THRESHOLD = -10;//-9.8;//-29;//-24;//-22;

const int NEXT_Q_TOO_LOW_ERROR = -99999;

const int INPUT_SIZE = 834;//1058;
const int HIDDEN_LAYER_1_SIZE = 520;//700;
const int HIDDEN_LAYER_2_SIZE = 400;//500;

const int MAP_SECTOR_SIZE = 2 * FOV_WIDTH;

const int MAX_ENEMIES_TO_TRACK = 6; /// CHANGE WOULD REQUIRE ADDITIONAL CHANGES IN OBSERVATION

const int ABSTRACT_SECTOR_SIZE = 9; /// must be integer

const int TILE_SCALE = 32;
const int LENGTH_UI_FRAME = TILE_SCALE * GRID_SPAN;

const int MAX_VISITED_FOR_STUCK = 3;//5;

const double POTENTIAL_FIELD_ATTRACTION_C = 10 * GRID_SPAN;
const double POTENTIAL_FIELD_REPULSION_ENEMY_N = 300; // 150 repels enemy more and hence better %reach. Advantage over A* baseline
const double POTENTIAL_FIELD_REPULSION_TRAIL_N = 20;
const int MAX_POTENTIAL_TRAIL_SIZE = 5;

const int ABSTRACT_SECTOR_PRA_STAR = 7;//16;

namespace RTS {
    enum LOG_LEVEL {
        INFO,
        DEBUG,
        OFF
    };
}

#endif //EXAMPLE_GAMECONSTANTS_H
