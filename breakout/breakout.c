#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>

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
int score = 0;
int lives = 3;
int ball_in_motion = 0;
int ball_dx = 1;  // ball movement direction x (1 for right, -1 for left)
int ball_dy = -1; // ball movement direction y (-1 for up, 1 for down)

// Terminal settings for non-blocking input
struct termios orig_termios;

void reset_terminal_mode()
{
    tcsetattr(0, TCSANOW, &orig_termios);
}

void set_conio_terminal_mode()
{
    struct termios new_termios;
    tcgetattr(0, &orig_termios);
    new_termios = orig_termios;
    new_termios.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(0, TCSANOW, &new_termios);
    atexit(reset_terminal_mode);
}

int kbhit()
{
    struct timeval tv = {0L, 0L};
    fd_set fds;
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}

int getch()
{
    int r;
    unsigned char c;
    if ((r = read(0, &c, sizeof(c))) < 0)
    {
        return r;
    }
    else
    {
        return c;
    }
}

void setup()
{
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
}

void draw()
{
    printf("\033[H\033[J"); // clear the screen using ANSI escape codes
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            int print = 0;
            if (y == paddle.y && x >= paddle.x && x < paddle.x + paddle.width)
            {
                printf("%c", paddle.symbol);
                print = 1;
            }

            if (y == ball.y && x == ball.x)
            {
                printf("%c", ball.symbol);
                print = 1;
            }

            for (int i = 0; i < num_blocks; i++)
            {
                if (blocks[i].active && y >= blocks[i].y && y < blocks[i].y + blocks[i].height &&
                    x >= blocks[i].x && x < blocks[i].x + blocks[i].width)
                {
                    printf("%c", blocks[i].symbol);
                    print = 1;
                    break;
                }
            }

            if (!print)
                printf(" ");
        }
        printf("\n");
    }
    printf("Score: %d  Lives: %d\n", score, lives);
}

void update()
{
    if (kbhit())
    {
        char key = getch();
        switch (key)
        {
        case 'a': // left
            paddle.x = max(0, paddle.x - 2);
            break;
        case 'd': // right
            paddle.x = min(WIDTH - paddle.width, paddle.x + 2);
            break;
        case ' ': // start/resume
            ball_in_motion = 1;
            ball_dy = -1;
            break;
        }
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
                score += 10;
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
            if (lives > 0)
            {
                printf("Press space to continue...\n");
                while (getch() != ' ')
                {
                }
            }
        }
    }
}

int main()
{
    set_conio_terminal_mode();
    setup();
    while (lives > 0 && num_blocks > 0)
    {
        draw();
        update();
        usleep(100000);
    }

    printf("Game Over! Final Score: %d\n", score);
    return 0;
}
