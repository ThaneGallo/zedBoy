#ifndef SNAKE_H
#define SNAKE_H

#define WIDTH 128
#define HEIGHT 32


typedef struct {
    int x, y, fruitX, fruitY, score;
    int tailX[100], tailY[100];
    int nTail;
    int dir;
    int gameOver;
} SnakeGame;

// extern SnakeGame *game;

void SnakeGame_setup();
void SnakeGame_draw();
void SnakeGame_input(int direction);
void SnakeGame_logic();
void SnakeGame_tick(int direction);

// GameFunction SnakeGame_setup_func = SnakeGame_setup;
// GameFunctionTick SnakeGame_tick_func = SnakeGame_tick;

// gameSetups[0] = SnakeGame_setup_func;
// gameTicks[0] = SnakeGame_tick_func;

#endif /* SNAKE_H */