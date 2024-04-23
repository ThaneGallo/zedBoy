#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>

#include "../../utils/draw.h"
#include "tetris.h"
#include "../../utils/gameConstants.h"

#define WIDTH 32
#define HEIGHT 8

__attribute__((constructor)) void registerTetris()
{

    if (game_count < MAX_GAMES)
        games[TETRIS] = tetris_setup;
    if (game_count < MAX_GAMES)
        gameTicks[TETRIS] = tetris_tick;
}

int board[HEIGHT][WIDTH] = {0};

int tetrisScore = 0;
int tetrisGameover = 0;

typedef struct
{
    int x, y;
    int shape[4][4];
} Tetromino;

Tetromino currentPiece, pieces[7];


void rotate_piece()//done
{
    int temp[4][4];
    memset(temp, 0, sizeof(temp));

    // Store the current shape in a temporary array rotated clockwise
    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 4; x++)
        {
            temp[x][3 - y] = currentPiece.shape[y][x];
        }
    }

    // Save the old shape in case we need to revert
    int old_shape[4][4];
    memcpy(old_shape, currentPiece.shape, sizeof(currentPiece.shape));

    // Apply the rotated shape to the current piece
    memcpy(currentPiece.shape, temp, sizeof(currentPiece.shape));

    // Check for collision with the new shape
    if (check_collision(currentPiece.x, currentPiece.y))
    {
        // Revert to old shape if there is a collision
        memcpy(currentPiece.shape, old_shape, sizeof(currentPiece.shape));
    }
}

int check_collision(int nx, int ny)//
{
    for (int dy = 0; dy < 4; dy++)
    {
        for (int dx = 0; dx < 4; dx++)
        {
            if (currentPiece.shape[dy][dx])
            { // Check only occupied parts of the Tetromino
                int x = nx + dx;
                int y = ny + dy;
                if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT || board[y][x])
                {
                    return 1; // Collision detected
                }
            }
        }
    }
    return 0; // No collision
}

void merge_piece()
{
    for (int dy = 0; dy < 4; dy++)
    {
        for (int dx = 0; dx < 4; dx++)
        {
            if (currentPiece.shape[dy][dx] && (currentPiece.x + dx < WIDTH))
            {
                board[currentPiece.y + dy][currentPiece.x + dx] = 1;
            }
        }
    }
}

void clear_lines()
{
    for (int x = WIDTH - 1; x >= 0; x--)
    {
        int complete = 1;
        for (int y = 0; y < HEIGHT; y++)
        {
            if (!board[y][x])
            {
                complete = 0;
                break;
            }
        }
        if (complete)
        {
            tetrisScore += 10;
            for (int tx = x; tx > 0; tx--)
            {
                for (int y = 0; y < HEIGHT; y++)
                {
                    board[y][tx] = board[y][tx - 1];
                }
            }
            for (int y = 0; y < HEIGHT; y++)
            {
                board[y][0] = 0;
            }
            x++; // Since we are clearing this column, check it again in case the above line filled it
        }
    }
}

void init_pieces()
{
    // Tetromino shapes
    int shapes[7][4][4] = {
        {{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}}, // I
        {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}}, // O
        {{0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, // T
        {{0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, // S
        {{1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, // Z
        {{1, 0, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, // J
        {{0, 0, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}  // L
    };
    for (int i = 0; i < 7; i++)
    {
        memcpy(pieces[i].shape, shapes[i], sizeof(pieces[i].shape));
    }
}

void init_piece()
{
    currentPiece.x = 0;              // Start from the far left of the board
    currentPiece.y = HEIGHT / 2 - 2; // Vertically centered
    int r = rand() % 7;
    memcpy(currentPiece.shape, pieces[r].shape, sizeof(currentPiece.shape));
    if (check_collision(currentPiece.x, currentPiece.y))
    {
        printf("\nGame Over!\n");
        exit(0);
    }
}

void tetris_draw()
{
    clearScreen(fd);
    for (int x = 0; x < WIDTH; x++)
    {
        for (int y = 0; y < HEIGHT; y++)
        {
            int pieceOccupied = 0;
            if (board[y][x])
            {
                for (int dx = 0; dx < 4; dx++)
                {
                    for (int dy = 0; dy < 4; dy++)
                    {
                        drawPixel(4 * x + dx, 4 * y + dy, 1);
                    }
                }
            }
            for (int dy = 0; dy < 4; dy++)
            {
                for (int dx = 0; dx < 4; dx++)
                {
                    if (currentPiece.shape[dy][dx] && x == currentPiece.x + dx && y == currentPiece.y + dy)
                    {
                        pieceOccupied = 1;
                        break;
                    }
                }
                if (pieceOccupied)
                {
                    for (int dx = 0; dx < 4; dx++)
                    {
                        for (int dy = 0; dy < 4; dy++)
                        {
                            drawPixel(4 * x + dx, 4 * y + dy, 1); // Draw each block as a 4x4 pixel block
                        }
                    }
                    break;
                }
            }
        }
    }
    sendBuffer(fd, buf);
}

void tetris_input(int direction)
{

    switch (direction)
    {
    case DOWN: // Move left

        if (currentPiece.y > 0 && !check_collision(currentPiece.x, currentPiece.y + 1))
        {
            currentPiece.y++; // Move the piece one unit to the left if no collision and within bounds
        }
        break;
    case UP: // Move right

        if (currentPiece.y + getMaxWidth(currentPiece.shape) - 1 < HEIGHT - 1 && !check_collision(currentPiece.x, currentPiece.y - 1))
        {
            currentPiece.y--; // Move the piece one unit to the right if no collision and within bounds
        }
        break;

    case MIDDLE: // Rotate clockwise
        rotate_piece();
        break;
    }

    // Automatic progression to the right every cycle
    static int move_counter = 0;
    move_counter++;
    if (move_counter >= 1)
    {
        move_counter = 0;
        if (currentPiece.x + 3 < WIDTH && !check_collision(currentPiece.x + 1, currentPiece.y))
        {
            currentPiece.x++; // Move the piece one unit to the right
        }
        else
        {
            merge_piece();
            clear_lines();
            init_piece();
        }
    }
}

// Utility to get the maximum width of the current tetromino to manage boundaries
int getMaxWidth(int shape[4][4])
{
    int maxWidth = 0;
    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 4; x++)
        {
            if (shape[y][x] && y + 1 > maxWidth)
            {
                maxWidth = y + 1;
            }
        }
    }
    return maxWidth;
}

void tetris_setup(){
    srand(time(NULL));
    init_pieces();
    init_piece();

    showControls(fd, 3);
};


int tetris_tick(int direction)
{
     if(tetrisGameover==1){
        tetrisGameover=0;
        return tetrisGameover;
    }
        tetris_draw();
        tetris_input(direction);

        return 0;
}
