#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <errno.h>

#include <libzed/axi_gpio.h>
#include <libzed/zed_common.h>
#include <libzed/zed_oled.h>

#define WIDTH 128
#define HEIGHT 32

int i, j, gameOver;
int x, y, fruitX, fruitY, score;
int tailX[1024], tailY[1024];
int nTail;
enum eDirection
{
    STOP = 0,
    LEFT,
    RIGHT,
    UP,
    DOWN
};
enum eDirection dir;

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

void setup()
{
    gameOver = 0;
    dir = STOP;
    x = WIDTH / 2;
    y = HEIGHT / 2;
    fruitX = rand() % WIDTH;
    fruitY = rand() % HEIGHT;
    score = 0;
}

void draw(struct zedoled_data *inst)
{
    // clear display
    zedoled_clear(inst);

    // draw fruit
    zedoled_set_pixel(inst, fruitX, fruitY, 1);

    // draw snake's tail
    for (int k = 0; k < nTail; k++)
    {
        zedoled_set_pixel(inst, tailX[k], tailY[k], 1);
    }

    // draw snake's head
    zedoled_set_pixel(inst, x, y, 1);

    // update display
    zedoled_update(inst);
}

void input()
{
    if (keyhit())
    {
        switch (getchar())
        {
        case 'a':
            if (dir != RIGHT)
            {
                dir = LEFT;
            }
            break;
        case 'd':
            if (dir != LEFT)
            {
                dir = RIGHT;
            }
            break;
        case 'w':
            if (dir != DOWN)
            {
                dir = UP;
            }
            break;
        case 's':
            if (dir != UP)
            {
                dir = DOWN;
            }
            break;
        case 'x':
            gameOver = 1;
            break;
        }
    }
}

void logic()
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
        if (tailX[i] == x && tailY[i] == y)
            gameOver = 1;

    if (x == fruitX && y == fruitY)
    {
        score += 10;
        fruitX = rand() % WIDTH;
        fruitY = rand() % HEIGHT;
        nTail++;
    }
}

int main()
{
    struct zedoled_data *inst;
    int ret;

    inst = zedoled_get();

    ret = zedoled_initialize(inst);

    if (ret != 0)
    {
        printf("OLED init failed");
        return EINVAL;
    }

    srand(time(0));
    setup();
    while (!gameOver)
    {
        draw(inst);
        input();
        logic();
        usleep(100000);
    }
    return 0;
}