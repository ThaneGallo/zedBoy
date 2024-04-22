#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <errno.h>

#include "snake.h"
#include "draw.h"
#include "../utils/gameConstants.h"



#define WIDTH 128
#define HEIGHT 32

int snakeoledfd;

SnakeGame *game;








__attribute__((constructor))
void registerSnakeGames() {
    if (game_count < MAX_GAMES) games[SNAKE] = SnakeGame_setup;
    if (game_count < MAX_GAMES) gameTicks[SNAKE] = SnakeGame_tick;
}


//place to print updates to

int keyhit(void)
{
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

void SnakeGame_setup(int oledfd)
{
    snakeoledfd = oledfd;
    game = (SnakeGame *)malloc(sizeof(SnakeGame));
    game->gameOver = 0;
    game->dir = STOP;
    game->x = WIDTH / 2;
    game->y = HEIGHT / 2;
    game->fruitX = rand() % WIDTH;
    game->fruitY = rand() % HEIGHT;
    game->score = 0;
    game->nTail = 0;
}

void SnakeGame_draw(int fd)
{

    clearScreen(fd);

    // draw fruit
    drawPixel(game->fruitX, game->fruitY, 1);

    // draw snake's tail
    for (int k = 0; k < game->nTail; k++)
    {
        drawPixel(game->tailX[k], game->tailY[k], 1);
    }

    // draw snake's head
    drawPixel(game->x, game->y, 1);

    // update display
    sendBuffer(fd, buf);
}

void SnakeGame_input( int direction) {
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

void input()
{
    if (keyhit())
    {
        switch (getchar())
        {
        case 'a':
            if (game->dir != RIGHT)
            {
                game->dir = LEFT;
            }
            break;
        case 'd':
            if (game->dir != LEFT)
            {
                game->dir = RIGHT;
            }
            break;
        case 'w':
            if (game->dir != DOWN)
            {
                game->dir = UP;
            }
            break;
        case 's':
            if (game->dir != UP)
            {
                game->dir = DOWN;
            }
            break;
        case 'x':
            game->gameOver = 1;
            break;
        }
    }
}

void SnakeGame_logic()
{
    int i;

    int prevX = game->tailX[0];
    int prevY = game->tailY[0];
    int prev2X, prev2Y;
    game->tailX[0] = game->x;
    game->tailY[0] = game->y;
    for ( i = 1; i < game->nTail; i++)
    {
        prev2X = game->tailX[i];
        prev2Y = game->tailY[i];
        game->tailX[i] = prevX;
        game->tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }
    switch (game->dir)
    {
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
    if (game->x >= WIDTH)
        game->x = 0;
    else if (game->x < 0)
        game->x = WIDTH - 1;
    if (game->y >= HEIGHT)
        game->y = 0;
    else if (game->y < 0)
        game->y = HEIGHT - 1;

    for (i = 0; i < game->nTail; i++)
        if (game->tailX[i] == game->x && game->tailY[i] == game->y)
            game->gameOver = 1;

    if (game->x == game->fruitX && game->y == game->fruitY)
    {
        game->score += 10;
        game->fruitX = rand() % WIDTH;
        game->fruitY = rand() % HEIGHT;
        game->nTail++;
        game->nTail++;
    }
}

void SnakeGame_tick(int direction) {
    SnakeGame_input(direction);
    SnakeGame_logic();
    SnakeGame_draw(snakeoledfd);
}