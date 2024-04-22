#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

#include "Snake.h"


void SnakeGame_setup(SnakeGame *game) {
    game->gameOver = 0;
    game->dir = STOP;
    game->x = WIDTH / 2;
    game->y = HEIGHT / 2;
    game->fruitX = rand() % WIDTH;
    game->fruitY = rand() % HEIGHT;
    game->score = 0;
    game->nTail = 0;
}

void SnakeGame_draw(const SnakeGame *game) {
    // Implementation of draw function
    // Output the game state
    int i, j, k;

    // system("clear");
    for (i = 0; i < WIDTH + 2; i++) {
        printf("#");
        // printk(KERN_INFO "#");
    }
    printf("\n");
    // printk(KERN_INFO "\n");

    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++) {
            if (j == 0) {
                printf("#");
                // printk(KERN_INFO "#");
            }

            if (i == game->y && j == game->x) {
                printf("O");
                // printk(KERN_INFO "O");
            } else if (i == game->fruitY && j == game->fruitX) {
                printf("F");
                // printk(KERN_INFO "F");
            } else {
                int print = 0;
                for (k = 0; k < game->nTail; k++) {
                    if (game->tailX[k] == j && game->tailY[k] == i) {
                        printf("o");
                        // printk(KERN_INFO "o");
                        print = 1;
                    }
                }
                if (!print) {
                    printf(" ");
                    // printk(KERN_INFO " ");
                }
            }

            if (j == WIDTH - 1) {
                printf("#");
                // printk(KERN_INFO "#");
            }
        }
        printf("\n");
        // printk(KERN_INFO "\n");
    }

    for (i = 0; i < WIDTH + 2; i++) {
        printf("#");
        // printk(KERN_INFO "#");
    }
    printf("\n");
    printf("Score: %d\n", game->score);
    // printk(KERN_INFO "\n");
    // printk(KERN_INFO "Score: %d\n", game->score);
}


void SnakeGame_input(SnakeGame *game, int direction) {
    // Input handling based on GPIO interrupt
    // Set direction accordingly

    switch (direction) {
        case 1:
            if (game->dir != RIGHT) {
                game->dir = LEFT;
            }
            break;
        case 2:
            if (game->dir != LEFT) {
                game->dir = RIGHT;
            }
            break;
        case 3:
            if (game->dir != DOWN) {
                game->dir = UP;
            }
            break;
        case 4:
            if (game->dir != UP) {
                game->dir = DOWN;
            }
            break;
        default:
            // if stop button is pressed, end the game (DIR = 0 = STOP)
            game->gameOver = 1;
            break;
    }
}

void SnakeGame_logic(SnakeGame *game) {
    // Update game logic
    int i;

    int prevX = game->tailX[0];
    int prevY = game->tailY[0];
    int prev2X, prev2Y;

    // Update the tail position
    game->tailX[0] = game->x;
    game->tailY[0] = game->y;

    for (i = 1; i < game->nTail; i++) {
        prev2X = game->tailX[i];
        prev2Y = game->tailY[i];
        game->tailX[i] = prevX;
        game->tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }

    // Update the head position based on the direction
    switch (game->dir) {
        case LEFT:
            game->x--;
            break;
        case RIGHT:
            game->x++;
            break;
        case UP:
            game->y--;
            break;
        case DOWN:
            game->y++;
            break;
        default:
            break;
    }

    // lapping around the screen
    if (game->x >= WIDTH) {
        game->x = 0;
    } else if (game->x < 0) {
        game->x = WIDTH - 1;
    }

    if (game->y >= HEIGHT) {
        game->y = 0;
    } else if (game->y < 0) {
        game->y = HEIGHT - 1;
    }

    // Check if the snake has collided with itself
    for (i = 0; i < game->nTail; i++) {
        if (game->tailX[i] == game->x && game->tailY[i] == game->y) {
            game->gameOver = 1;
        }
    }

    // Check if the snake has eaten the fruit
    if (game->x == game->fruitX && game->y == game->fruitY) {
        game->score += 10;
        game->fruitX = rand() % WIDTH;
        game->fruitY = rand() % HEIGHT;
        game->nTail++;
    }
}
