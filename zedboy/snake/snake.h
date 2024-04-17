#ifndef SNAKE_H
#define SNAKE_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include "../gameConstants.h"

#define WIDTH 20
#define HEIGHT 20


typedef struct {
    int x, y, fruitX, fruitY, score;
    int tailX[100], tailY[100];
    int nTail;
    enum Direction dir;
    int gameOver;
} SnakeGame;

void SnakeGame_setup(SnakeGame *game);
void SnakeGame_draw(const SnakeGame *game);
void SnakeGame_input(SnakeGame *game, int direction);
void SnakeGame_logic(SnakeGame *game);
void SnakeGame_tick(SnakeGame *game, int direction);

GameFunction SnakeGame_setup_func = SnakeGame_setup;
GameFunctionTick SnakeGame_tick_func = SnakeGame_tick;

gameSetups[0] = SnakeGame_setup_func;
gameTicks[0] = SnakeGame_tick_func;

#endif /* SNAKE_H */