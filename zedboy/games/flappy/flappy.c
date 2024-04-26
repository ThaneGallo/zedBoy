#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>

#include "../../utils/draw.h"
#include "../../utils/gameConstants.h"

#define WIDTH 128
#define HEIGHT 32
#define GAP_SIZE 7
#define WALL_GAP 25
#define NUM_WALLS 6

int gameOver;
int birdY, wallXs[NUM_WALLS], gaps[NUM_WALLS], score;

void setup()
{
    gameOver = 0;
    birdY = HEIGHT / 2;
    score = 0;
    srand(time(NULL)); // Seed random number generator
    for (int i = 0; i < NUM_WALLS; i++)
    {
        wallXs[i] = WIDTH - (4 * WALL_GAP) + (i * WALL_GAP);
        gaps[i] = rand() % (HEIGHT - GAP_SIZE - 1) + 1;
    }
}

void input(int direction)
{
    switch (direction)
    {
        case UP:
            birdY -= 2;
            break;
    }
}

void logic()
{
    birdY++; // Gravity effect

    // Wall movement and collision detection
    for (int i = 0; i < NUM_WALLS; i++)
    {
        wallXs[i]--;

        if (wallXs[i] < 0)
        {
            wallXs[i] = WIDTH - 1;
            gaps[i] = rand() % (HEIGHT - GAP_SIZE - 1) + 1;
            score++;
        }

        // Check for collisions
        if (wallXs[i] == 2 && (birdY < gaps[i] || birdY > gaps[i] + GAP_SIZE))
        {
            gameOver = 1;
        }
    }

    // Check game boundaries
    if (birdY < 0 || birdY >= HEIGHT)
        gameOver = 1;
}

void draw(int fd)
{
    clearScreen(fd);

    // Draw walls
    for (int k = 0; k < NUM_WALLS; k++)
    {

        for (int y = 0; y < HEIGHT; y++)
        {
            if (y < gaps[k] || y > gaps[k] + GAP_SIZE)
            {
                drawPixel(wallXs[k], y, 1); // Draw walls
            }
        }
    }

    // Draw bird
    drawPixel(2, birdY, 1); // Bird is always at x position 2

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
    }

    printf("Game Over! Your score was: %d\n", score);
    oledClose(fd);
    return 0;
}
