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

int i, j, Snakegame_over;
int x, y, fruitX, fruitY, Snakegame_score;
int tailX[1024], tailY[1024];
int nTail = 0;;
int dir;

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

void SnakeGame_setup()
{
    Snakegame_over = 0;
    dir = STOP;
    x = WIDTH / 2;
    y = HEIGHT / 2;
    fruitX = rand() % WIDTH;
    fruitY = rand() % HEIGHT;
    Snakegame_score = 0;
    nTail = 0;
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

void SnakeGame_input(int direction)
{
        switch (direction)
        {
        case LEFT:
            if (dir != RIGHT)
            {
                dir = LEFT;
            }
            break;
        case RIGHT:
            if (dir != LEFT)
            {
                dir = RIGHT;
            }
            break;
        case UP:
            if (dir != DOWN)
            {
                dir = UP;
            }
            break;
        case DOWN:
            if (dir != UP)
            {
                dir = DOWN;
            }
            break;
        case -1:
            Snakegame_over = 1;
            break;
        }
    }


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
            Snakegame_over = 1;
        }

    if (x == fruitX && y == fruitY)
    {
        Snakegame_score += 10;
        fruitX = rand() % WIDTH;
        fruitY = rand() % HEIGHT;
        nTail++;
    }
}

int SnakeGame_tick(int direction){
    if(Snakegame_over==1){
        printf("returning gameover %d\n",gameOver);
        Snakegame_over=0;
        return Snakegame_score;
    }
        SnakeGame_draw();
        SnakeGame_input(direction);
        SnakeGame_logic();

        return 0;



}