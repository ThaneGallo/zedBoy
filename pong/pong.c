#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>
#include "draw.h"

#define WIDTH 40
#define HEIGHT 10

int ballX = WIDTH / 2, ballY = HEIGHT / 2;
int velocityX = -1, velocityY = 1;
int playerPaddleY = HEIGHT / 2;
int AIPaddleY = HEIGHT / 2;
int playerScore = 0;
int aiScore = 0;
int gameOver = 0;

void setup()
{
    srand(time(NULL));
    playerPaddleY = HEIGHT / 2;
    AIPaddleY = HEIGHT / 2;
    ballX = WIDTH / 2;
    ballY = HEIGHT / 2;
    velocityX = -1;
    velocityY = 1;
    playerScore = 0;
    aiScore = 0;
    gameOver = 0;
}

int keyhit()
{
    struct termios oldt, newt;
    int ch;
    int oldf = fcntl(STDIN_FILENO, F_GETFL, 0);

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
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

void input()
{
    if (keyhit())
    {
        char ch = getchar();
        switch (ch)
        {
        case 'w':
            if (playerPaddleY > 0)
                playerPaddleY--;
            break;
        case 's':
            if (playerPaddleY < HEIGHT - 4)
                playerPaddleY++;
            break;
        }
    }
}

void logic()
{
    ballX += velocityX;
    ballY += velocityY;

    // Ball collision with top or bottom
    if (ballY <= 0 || ballY >= HEIGHT - 1)
    {
        velocityY = -velocityY;
    }

    // Ball collision with paddles
    if (ballX == 2 && (ballY >= playerPaddleY && ballY < playerPaddleY + 4))
    {
        velocityX = -velocityX;
    }
    else if (ballX == WIDTH - 3 && (ballY >= AIPaddleY && ballY < AIPaddleY + 4))
    {
        velocityX = -velocityX;
    }

    // Scoring and reset
    if (ballX <= 1 || ballX >= WIDTH - 2)
    {
        if (ballX <= 1)
            aiScore++;
        else
            playerScore++;

        ballX = WIDTH / 2;
        ballY = HEIGHT / 2;
        velocityX = -velocityX;
    }

    // AI movement
    if (AIPaddleY < ballY - 1)
    {
        AIPaddleY++;
    }
    else if (AIPaddleY > ballY + 1)
    {
        AIPaddleY--;
    }

    // Prevent the AI paddle from moving out of bounds
    if (AIPaddleY < 0)
    {
        AIPaddleY = 0;
    }
    else if (AIPaddleY > HEIGHT - 4)
    {
        AIPaddleY = HEIGHT - 4;
    }
}

void draw(int fd)
{
    clearScreen(fd);

    // Draw player paddle as a 2x2 block for each unit
    for (int i = 0; i < 4; ++i)
    {
        for (int dx = 0; dx < 2; dx++)
        {
            for (int dy = 0; dy < 2; dy++)
            {
                drawPixel(2 * 1 + dx, 2 * (playerPaddleY + i) + dy, 1);
            }
        }
    }

    // Draw AI paddle as a 2x2 block for each unit
    for (int i = 0; i < 4; ++i)
    {
        for (int dx = 0; dx < 2; dx++)
        {
            for (int dy = 0; dy < 2; dy++)
            {
                drawPixel(2 * (WIDTH - 2) + dx, 2 * (AIPaddleY + i) + dy, 1);
            }
        }
    }

    // Draw ball as a 2x2 block
    for (int dx = 0; dx < 2; dx++)
    {
        for (int dy = 0; dy < 2; dy++)
        {
            drawPixel(2 * ballX + dx, 2 * ballY + dy, 1);
        }
    }

    // Update display
    sendBuffer(fd, buf);
}

int main()
{
    int fd = oledOpen();
    setup();
    while (!gameOver)
    {
        input();
        logic();
        draw(fd);
        usleep(1000); // Game speed
    }

    oledClose(fd);
    return 0;
}