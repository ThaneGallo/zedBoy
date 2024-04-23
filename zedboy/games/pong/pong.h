#ifndef PONG_H
#define PONG_H

// extern SnakeGame *game;
void drawScore();
void pong_setup();
void pong_draw();
void pong_input(int direction);
void pong_logic();
int pong_tick(int direction);


// GameFunction SnakeGame_setup_func = SnakeGame_setup;
// GameFunctionTick SnakeGame_tick_func = SnakeGame_tick;

// gameSetups[0] = SnakeGame_setup_func;
// gameTicks[0] = SnakeGame_tick_func;

#endif /* SNAKE_H */