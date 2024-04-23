#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>

#include "../../utils/draw.h"
#include "../../utils/gameConstants.h"
#include "pong.h"

#define WIDTH 32
#define HEIGHT 16
#define WIN_THRESH 5

int ballX = WIDTH / 2, ballY = HEIGHT / 2;
int velocityX = -1, velocityY = 1;
int playerPaddleY = HEIGHT / 2;
int AIPaddleY = HEIGHT / 2;
int playerScore = 0;
int aiScore = 0;
int pongDir;

__attribute__((constructor))
void registerPong() {

    if (game_count < MAX_GAMES) games[PONG] = pong_setup;
    if (game_count < MAX_GAMES) gameTicks[PONG] = pong_tick;
}

void pong_setup()
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

    showControls(fd, 1);
}


void drawScore()
{

    char playerScoreChar = '0' + playerScore;
    char aiScoreChar = '0' + aiScore;

    drawLine(WIDTH * 2 + 2, 0, WIDTH * 2 + 2, 32, 1);

    drawLine(WIDTH * 2 + 2, 8, OLED_WIDTH, 8, 1);
    drawLine(WIDTH * 2 + 2, 15, OLED_WIDTH, 15, 1);

    drawLine(((WIDTH * 2 + 2) + OLED_WIDTH) / 2, 8, ((WIDTH * 2 + 2) + OLED_WIDTH) / 2, 32, 1);

    drawCharacter('s', 18 + 0 + (WIDTH * 2 + 2), 1);
    drawCharacter('c', 18 + 5 + (WIDTH * 2 + 2), 1);
    drawCharacter('o', 18 + 10 + (WIDTH * 2 + 2), 1);
    drawCharacter('r', 18 + 15 + (WIDTH * 2 + 2), 1);
    drawCharacter('e', 18 + 20 + (WIDTH * 2 + 2), 1);
    drawCharacter(':', 18 + 25 + (WIDTH * 2 + 2), 1);

    drawCharacter('a', 5 + ((WIDTH * 2 + 2) + OLED_WIDTH) / 2, 9);
    drawCharacter('i', 10 + ((WIDTH * 2 + 2) + OLED_WIDTH) / 2, 9);

    drawCharacter('u', 5 + (WIDTH * 2 + 2), 9);
    drawCharacter('s', 10 + (WIDTH * 2 + 2), 9);
    drawCharacter('e', 15 + (WIDTH * 2 + 2), 9);
    drawCharacter('r', 20 + (WIDTH * 2 + 2), 9);

    // draws scores
    drawCharacter(playerScoreChar, 80, 22);
    drawCharacter(aiScoreChar, 112, 22);
}

void pong_input(int direction)
{

     switch (direction)
        {
        case UP:
                 if (playerPaddleY > 0)
                playerPaddleY--;
            break;
        case DOWN:
                 if (playerPaddleY < HEIGHT - 4)
                playerPaddleY++;
            break;

        case -1:
            break;
        }
}

void pong_logic()
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



void pong_draw()
{
    clearScreen(fd);

    char playerScoreChar = (char)playerScore;
    char aiScoreChar = (char)aiScore;

    drawScore();

    // drawWord("score:", 0);
    // drawWord(playerScore,0);
    // drawWord(aiScore,0);

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

int pong_tick(int direction)
{

    if (playerScore >= WIN_THRESH) //win
    {
        return 1;
    }
    else if (aiScore >= WIN_THRESH) // lose
    {
        return -1;
    }


    pong_draw();
    pong_input(direction);
    pong_logic();

    return 0;
}
