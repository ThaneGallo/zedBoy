#ifndef TETRIS_H
#define TETRIS_H

// extern SnakeGame *game;

void tetris_setup();
void rotate_piece();
int check_collision(int nx, int ny);
void merge_piece();
void clear_lines();
void init_pieces();
void init_piece();
int getMaxWidth(int shape[4][4]);
void tetris_draw();
void tetris_input(int direction);
void tetris_logic();

int tetris_tick(int direction);


// GameFunction SnakeGame_setup_func = SnakeGame_setup;
// GameFunctionTick SnakeGame_tick_func = SnakeGame_tick;

// gameSetups[0] = SnakeGame_setup_func;
// gameTicks[0] = SnakeGame_tick_func;

#endif /* SNAKE_H */