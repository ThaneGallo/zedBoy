#ifndef SNAKE_H
#define SNAKE_H

// #include <stdio.h>
// #include <stdlib.h>
// #include <time.h>
// #include <unistd.h>
// #include <termios.h>
// #include <fcntl.h>

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/gpio/consumer.h>
#include <linux/gpio/driver.h>
#include <linux/device.h>
#include <linux/hrtimer.h>
#include <linux/of_device.h>
#include <asm/io.h>
#include <linux/random.h> // Include this header for prandom_u32_max

#define WIDTH 20
#define HEIGHT 20

enum Direction {
    STOP = 0,
    LEFT = 1,
    RIGHT = 2,
    UP = 3,
    DOWN = 4
};

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

#endif /* SNAKE_H */