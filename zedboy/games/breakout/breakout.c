
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>


#include "breakout.h"
#include "../../utils/draw.h"
#include "../../utils/gameConstants.h"

__attribute__((constructor))
void registerBreakoutGame() {
     game_count++;

    if (game_count < MAX_GAMES) games[BREAKOUT] = Breakout_setup;
    if (game_count < MAX_GAMES) gameTicks[BREAKOUT] = Breakout_tick;
}

#define WIDTH 64
#define HEIGHT 16
#define PADDLE_WIDTH 8

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

typedef struct
{
    int x, y;
    int width, height;
    char symbol;
    int active;
} GameObject;

GameObject paddle;
GameObject ball;
GameObject blocks[32];
int num_blocks = 32;
int breakout_score = 0;
int lives = 3;
int ball_in_motion = 0;
int ball_dx = 1;  // ball movement direction x (1 for right, -1 for left)
int ball_dy = -1; // ball movement direction y (-1 for up, 1 for down)



void Breakout_setup()
{



    num_blocks = 32;
    breakout_score = 0;
    lives = 3;
    ball_in_motion = 0;
    ball_dx = 1;  // ball movement direction x (1 for right, -1 for left)

    paddle.x = WIDTH / 2 - PADDLE_WIDTH / 2;
    paddle.y = HEIGHT - 2;
    paddle.width = PADDLE_WIDTH;
    paddle.height = 1;
    paddle.symbol = '=';

    ball.x = WIDTH / 2;
    ball.y = HEIGHT - 3;
    ball.symbol = 'O';

    int block_width = 6;
    int block_height = 1;
    int padding = 1;
    int x_start = 2;
    int y_start = 2;
    int count = 0;
    
    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 8; x++)
        {
            blocks[count].x = x_start + x * (block_width + padding);
            blocks[count].y = y_start + y * (block_height + padding);
            blocks[count].width = block_width;
            blocks[count].height = block_height;
            blocks[count].symbol = '#';
            blocks[count].active = 1;
            count++;
        }
    }
    Breakout_draw();
}


void Breakout_draw()
{
    clearScreen(fd); // Clears the OLED buffer

    // Draw paddle
    for (int x = paddle.x; x < paddle.x + paddle.width; x++)
    {
        for (int dx = 0; dx < 2; dx++)
        {
            for (int dy = 0; dy < 2; dy++)
            {
                drawPixel(2 * x + dx, 2 * paddle.y + dy, 1);
            }
        }
    }

    for (int dx = 0; dx < 2; dx++)
    {
        for (int dy = 0; dy < 2; dy++)
        {
            drawPixel(2 * ball.x + dx, 2 * ball.y + dy, 1);
        }
    }


    for (int i = 0; i < num_blocks; i++)
    {
        if (blocks[i].active)
        {
            for (int x = blocks[i].x; x < blocks[i].x + blocks[i].width; x++)
            {
                for (int y = blocks[i].y; y < blocks[i].y + blocks[i].height; y++)
                {
                    for (int dx = 0; dx < 2; dx++)
                    {
                        for (int dy = 0; dy < 2; dy++)
                        {
                           drawPixel(2 * x + dx, 2 * y + dy, 1);
            
                        }

                    }
                }
            }
        }
    }

    // Send the buffer to the display
    sendBuffer(fd, buf);
}

void Breakout_update(int direction)
{
        switch (direction)
        {
        case LEFT: // left
            paddle.x = max(0, paddle.x - 2);
            break;
        case RIGHT: // right
            paddle.x = min(WIDTH - paddle.width, paddle.x + 2);
            break;
        case MIDDLE: // start/resume
        if(!ball_in_motion){
            ball_in_motion = 1;
            ball_dy = -1;
            }
            break;
        }

    if (ball_in_motion)
    {
        // move ball
        ball.x += ball_dx;
        ball.y += ball_dy;

        // wall collision sides
        if (ball.x <= 0 || ball.x >= WIDTH - 1)
        {
            ball_dx = -ball_dx;
        }
        // wall collision top
        if (ball.y <= 0)
        {
            ball_dy = -ball_dy;
        }

        // collision with paddle
        if ((ball.y == paddle.y - 1 || (ball_dy > 0 && ball.y == paddle.y - 2)) &&
            (ball.x >= paddle.x && ball.x < paddle.x + paddle.width))
        {
            ball_dy = -ball_dy;
        }

        // collision with blocks
        for (int i = 0; i < num_blocks; i++)
        {
            if (blocks[i].active && ball.y >= blocks[i].y && ball.y < blocks[i].y + blocks[i].height &&
                ball.x >= blocks[i].x && ball.x < blocks[i].x + blocks[i].width)
            {
                blocks[i].active = 0;
                ball_dy = -ball_dy;
                breakout_score += 10;
                break;
            }
        }

        // ball falls below paddle
        if (ball.y >= HEIGHT)
        {
            lives--;
            ball_in_motion = 0;
            ball.x = WIDTH / 2;
            ball.y = HEIGHT - 3;
           
        }
    }
}

int Breakout_tick(int direction){

     if (lives > 0 && num_blocks > 0)
    {
        Breakout_update(direction);
        Breakout_draw();
        
        return 0;
    }
    return breakout_score;

}
