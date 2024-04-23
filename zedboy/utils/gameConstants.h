// gameConstants.h
#ifndef GAME_CONSTANTS_H
#define GAME_CONSTANTS_H

// Define game constants
// #define MAX_SCORE 100
// #define INITIAL_SPEED 5

typedef void (*GameFunction)();
typedef void (*GameFunctionTick)(int);

// GameFunction gameSetups[9];
// GameFunctionTick gameTicks[9];

// Declare an array of function pointers with a predefined maximum size
#define MAX_GAMES 10

// to track the number of games added
extern int game_count;  

extern int gameTick_count;  // to track the number of game ticks added

// to tell system that game is overs
extern int endGame; 

extern GameFunction games[MAX_GAMES];
extern GameFunctionTick gameTicks[MAX_GAMES];


enum GameIdx {
    SNAKE = 0,
    PONG = 1,
    BREAKOUT = 2,
    TETRIS = 3,
};

enum Direction {
    STOP = 0,
    LEFT = 1,
    RIGHT = 2,
    UP = 3,
    DOWN = 4,
    MIDDLE = 5,
};

#endif // GAME_CONSTANTS_H