#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <errno.h>

#include "../../utils/draw.h"
#include "snake.h"
#include"../../utils/gameConstants.h"

#define WIDTH 64
#define HEIGHT 16

__attribute__((constructor))
void registerSnakeGames() {

    if (game_count < MAX_GAMES) games[SNAKE] = SnakeGame_setup;
    if (game_count < MAX_GAMES) gameTicks[SNAKE] = SnakeGame_tick;
}

int i, j, gameOver;
int x, y, fruitX, fruitY, SnakeGame_score;
int tailX[1024], tailY[1024];
int nTail = 0;;
int dir;

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

void SnakeGame_setup()
{
    game = (SnakeGame *)malloc(sizeof(SnakeGame));
    game->gameOver = 0;
    game->dir = STOP;
    game->x = WIDTH / 2;
    game->y = HEIGHT / 2;
    game->fruitX = rand() % WIDTH;
    game->fruitY = rand() % HEIGHT;
    game->score = 100;
    game->nTail = 0;

}

void SnakeGame_draw()
{
    clearScreen(fd);

    // Draw fruit
    for (int dx = 0; dx < 2; dx++)
    {
        for (int dy = 0; dy < 2; dy++)
        {
            drawPixel(2 * fruitX + dx, 2 * fruitY + dy, 1);
        }
    }

    // Draw snake's tail
    for (int k = 0; k < nTail; k++)
    {
        for (int dx = 0; dx < 2; dx++)
        {
            for (int dy = 0; dy < 2; dy++)
            {
                drawPixel(2 * tailX[k] + dx, 2 * tailY[k] + dy, 1);
            }
        }
    }

    // Draw snake's head
    for (int dx = 0; dx < 2; dx++)
    {
        for (int dy = 0; dy < 2; dy++)
        {
            drawPixel(2 * x + dx, 2 * y + dy, 1);
        }
    }

    // Update display
    sendBuffer(fd, buf);
}

// void input()
// {
//     if (keyhit())
//     {
//         switch (getchar())
//         {
//         case 'a':
//             if (game->dir != RIGHT)
//             {
//                 game->dir = LEFT;
//             }
//             break;
//         case 'd':
//             if (game->dir != LEFT)
//             {
//                 game->dir = RIGHT;
//             }
//             break;
//         case 'w':
//             if (game->dir != DOWN)
//             {
//                 game->dir = UP;
//             }
//             break;
//         case 's':
//             if (game->dir != UP)
//             {
//                 game->dir = DOWN;
//             }
//             break;
//         case 'x':
//             endGame = 1;
//             game->gameOver = 1;
//             break;
//         }
//     }
// }

void SnakeGame_logic()
{
    int prevX = tailX[0];
    int prevY = tailY[0];
    int prev2X, prev2Y;
    tailX[0] = x;
    tailY[0] = y;
    for (i = 1; i < nTail; i++)
    {
        prev2X = tailX[i];
        prev2Y = tailY[i];
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }
    switch (dir)
    {
    case LEFT:
        x--;
        break;
    case RIGHT:
        x++;
        break;
    case UP:
        y--;
        break;
    case DOWN:
        y++;
        break;
    default:
        break;
    }
    if (x >= WIDTH)
        x = 0;
    else if (x < 0)
        x = WIDTH - 1;
    if (y >= HEIGHT)
        y = 0;
    else if (y < 0)
        y = HEIGHT - 1;

    for (i = 0; i < nTail; i++)
        if (tailX[i] == x && tailY[i] == y) {
            printf("endGame snake\n");
            gameOver = 1;
        }

    if (x == fruitX && y == fruitY)
    {
        SnakeGame_score += 10;
        fruitX = rand() % WIDTH;
        fruitY = rand() % HEIGHT;
        nTail++;
    }
}

int SnakeGame_tick(int direction){
    if(gameOver==1){
        printf("returning gameover %d\n",gameOver);
        gameOver=0;
        return 1;
    }
        SnakeGame_draw();
        SnakeGame_input(direction);
        SnakeGame_logic();

        return 0;



}