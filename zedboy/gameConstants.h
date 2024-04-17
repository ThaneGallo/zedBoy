// gameConstants.h
#ifndef GAME_CONSTANTS_H
#define GAME_CONSTANTS_H

// Define game constants
// #define MAX_SCORE 100
// #define INITIAL_SPEED 5

typedef void (*GameFunction)();
typedef void (*GameFunctionTick)(int);

GameFunction gameSetups[9];
GameFunctionTick gameTicks[9];

enum Direction {
    STOP = 0,
    LEFT = 1,
    RIGHT = 2,
    UP = 3,
    DOWN = 4
};

#endif // GAME_CONSTANTS_H