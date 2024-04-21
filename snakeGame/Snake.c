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

#include "Snake.h"

// #define WIDTH 20
// #define HEIGHT 20

// enum Direction {
//     STOP = 0,
//     LEFT = 1,
//     RIGHT = 2,
//     UP = 3,
//     DOWN = 4
// };

// typedef struct {
//     int i, j, gameOver;
//     int x, y, fruitX, fruitY, score;
//     int tailX[100], tailY[100];
//     int nTail;
//     enum Direction dir;
// } SnakeGame;

void SnakeGame_setup(SnakeGame *game) {
    game->gameOver = 0;
    game->dir = STOP;
    game->x = WIDTH / 2;
    game->y = HEIGHT / 2;
    // game->fruitX = rand() % WIDTH;
    // game->fruitY = rand() % HEIGHT;
    game->fruitX = prandom_u32_max(WIDTH);
    game->fruitY = prandom_u32_max(HEIGHT);
    game->score = 0;
    game->nTail = 0;
}

// void SnakeGame_draw(const SnakeGame *game) {
//     // Implementation of draw function
//     // Output the game state
//     int i, j, k;

//     // system("clear");
//     for (i = 0; i < WIDTH + 2; i++) {
//         // printf("#");
//         printk(KERN_INFO "#");
//     }
//     // printf("\n");
//     printk(KERN_INFO "\n");

//     for (i = 0; i < HEIGHT; i++) {
//         for (j = 0; j < WIDTH; j++) {
//             if (j == 0) {
//                 // printf("#");
//                 printk(KERN_INFO "#");
//             }

//             if (i == game->y && j == game->x) {
//                 // printf("O");
//                 printk(KERN_INFO "O");
//             } else if (i == game->fruitY && j == game->fruitX) {
//                 // printf("F");
//                 printk(KERN_INFO "F");
//             } else {
//                 int print = 0;
//                 for (k = 0; k < game->nTail; k++) {
//                     if (game->tailX[k] == j && game->tailY[k] == i) {
//                         // printf("o");
//                         printk(KERN_INFO "o");
//                         print = 1;
//                     }
//                 }
//                 if (!print) {
//                     // printf(" ");
//                     printk(KERN_INFO " ");
//                 }
//             }

//             if (j == WIDTH - 1) {
//                 // printf("#");
//                 printk(KERN_INFO "#");
//             }
//         }
//         // printf("\n");
//         printk(KERN_INFO "\n");
//     }

//     for (i = 0; i < WIDTH + 2; i++) {
//         // printf("#");
//         printk(KERN_INFO "#");
//     }
//     // printf("\n");
//     // printf("Score: %d\n", game->score);
//     printk(KERN_INFO "\n");
//     printk(KERN_INFO "Score: %d\n", game->score);
// }

void SnakeGame_draw(const SnakeGame *game) {
    int i, j, k;
    // Since WIDTH is 20, we allocate 23 (+2 for the borders and +1 for '\0')
    char lineBuffer[23]; // Adjust the size if WIDTH changes

    // Top border
    memset(lineBuffer, '#', WIDTH + 2);
    lineBuffer[WIDTH + 2] = '\0';
    printk(KERN_INFO "%s", lineBuffer); // Remove \n to control new lines more precisely

    for (i = 0; i < HEIGHT; i++) {
        lineBuffer[0] = '#'; // Left border
        for (j = 0; j < WIDTH; j++) {
            // Fill game area
            if (i == game->y && j == game->x) {
                lineBuffer[j + 1] = 'O'; // Snake head
            } else if (i == game->fruitY && j == game->fruitX) {
                lineBuffer[j + 1] = 'F'; // Fruit
            } else {
                bool print = false;
                for (k = 0; k < game->nTail; k++) {
                    if (game->tailX[k] == j && game->tailY[k] == i) {
                        lineBuffer[j + 1] = 'o'; // Snake tail
                        print = true;
                        break;
                    }
                }
                if (!print) {
                    lineBuffer[j + 1] = ' '; // Empty space
                }
            }
        }
        lineBuffer[WIDTH + 1] = '#'; // Right border
        lineBuffer[WIDTH + 2] = '\0'; // Null-terminate the string
        printk(KERN_INFO "%s", lineBuffer); // Remove \n for the same reason
    }

    // Bottom border
    memset(lineBuffer, '#', WIDTH + 2);
    lineBuffer[WIDTH + 2] = '\0';
    printk(KERN_INFO "%s", lineBuffer); // Adjust for consistent formatting

    // Print score directly without using the buffer
    printk(KERN_INFO "Score: %d\n", game->score);
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
        // game->fruitX = rand() % WIDTH;
        // game->fruitY = rand() % HEIGHT;
        game->fruitX = prandom_u32_max(WIDTH);
        game->fruitY = prandom_u32_max(HEIGHT);
        game->nTail++;
    }
}

// // initialize module
// static int mymod_init(void)
// {
//     printk(KERN_INFO "Snake Game Player Module\n");
//     // Initialize snake game
//     // srand(time(0));
//     // SnakeGame game;
//     // SnakeGame_setup(&game);
//     // SnakeGame_draw(&game);
//     return 0;
// }

// // cleanup module
// static void mymod_exit(void)
// {
//     // cleanup
// }

// // declare initialization function
// module_init(mymod_init);
// module_exit(mymod_exit);

// MODULE_DESCRIPTION("Snake Game Player Module");
// MODULE_LICENSE("GPL");










// int keyhit(void)
// {
//     struct termios oldt, newt;
//     int ch;
//     int oldf;

//     tcgetattr(STDIN_FILENO, &oldt);
//     newt = oldt;
//     newt.c_lflag &= ~(ICANON | ECHO);
//     tcsetattr(STDIN_FILENO, TCSANOW, &newt);
//     oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
//     fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

//     ch = getchar();

//     tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
//     fcntl(STDIN_FILENO, F_SETFL, oldf);

//     if (ch != EOF)
//     {
//         ungetc(ch, stdin);
//         return 1;   
//     }

//     return 0;
// }

// int main() {
//     srand(time(0));
//     SnakeGame game;
//     SnakeGame_setup(&game);
//     while (!game.gameOver) {
//         SnakeGame_draw(&game);

//         if (keyhit()) {
//             int direction = 0;
//             char ch = getchar();

//             switch (ch)
//             {
//                 case 'a':
//                     direction = 1;
//                     break;
//                 case 'd':
//                     direction = 2;
//                     break;
//                 case 'w': 
//                     direction = 3;
//                     break;
//                 case 's':
//                     direction = 4;
//                     break;
//                 default:
//                     direction = 0;
//                     break;
//             }

//             SnakeGame_input(&game, direction);
//         }
        
//         SnakeGame_logic(&game);
//         usleep(100000);
//     }
//     return 0;
// }