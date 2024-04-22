#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>

#include "draw.h"

#define WIDTH 32
#define HEIGHT 8

int board[HEIGHT][WIDTH] = {0};

int score = 0;

typedef struct
{
    int x, y;
    int shape[4][4];
} Tetromino;

Tetromino currentPiece, pieces[7];
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

void rotate_piece(int clockwise)
{
    int temp[4][4];
    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 4; x++)
        {
            if (clockwise)
            {
                temp[x][3 - y] = currentPiece.shape[y][x]; // Rotate the piece clockwise
            }
        }
    }
    memcpy(currentPiece.shape, temp, sizeof(currentPiece.shape));
    // After rotation, check for collisions
    if (check_collision(currentPiece.x, currentPiece.y))
    {
        // If there's a collision, try rotating back to see if that resolves the collision
        // However, since we do not want counter-clockwise rotation, adjustments might be needed based on the board's state
    }
}

int check_collision(int nx, int ny)
{
    for (int dy = 0; dy < 4; dy++)
    {
        for (int dx = 0; dx < 4; dx++)
        {
            int x = nx + dx;
            int y = ny + dy;
            if (currentPiece.shape[dy][dx])
            {
                if (x >= WIDTH || y < 0 || y >= HEIGHT || (x >= 0 && board[y][x]))
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
            score += 10;
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

// void draw()
// {
//     printf("\033[H\033[J");
//     for (int y = 0; y < HEIGHT; y++)
//     {
//         for (int x = 0; x < WIDTH; x++)
//         {
//             if (board[y][x])
//             {
//                 printf("X");
//             }
//             else
//             {
//                 int printed = 0;
//                 for (int dy = 0; dy < 4; dy++)
//                 {
//                     for (int dx = 0; dx < 4; dx++)
//                     {
//                         if (currentPiece.shape[dy][dx] && y == currentPiece.y + dy && x == currentPiece.x + dx)
//                         {
//                             printf("X");
//                             printed = 1;
//                             break;
//                         }
//                     }
//                     if (printed)
//                         break;
//                 }
//                 if (!printed)
//                     printf(" ");
//             }
//         }
//         printf("\n");
//     }
// }

void draw(int fd)
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

void update()
{
    if (kbhit())
    {
        char key = getch();
        switch (key)
        {
        case 'a': // Move upwards
            if (!check_collision(currentPiece.x, currentPiece.y - 1))
            {
                currentPiece.y--; // Move the piece one unit upwards
            }
            break;
        case 'd': // Move downwards
            if (!check_collision(currentPiece.x, currentPiece.y + 1))
            {
                currentPiece.y++; // Move the piece one unit downwards
            }
            break;
        case ' ': // Rotate clockwise
            rotate_piece(1);
            if (check_collision(currentPiece.x, currentPiece.y))
            {
                rotate_piece(0); // Undo rotation if it results in a collision
            }
            break;
        }
    }

    static int move_counter = 0;
    move_counter++;
    if (move_counter >= 10)
    { // Time-based automatic progression to the right
        move_counter = 0;
        if (!check_collision(currentPiece.x + 1, currentPiece.y))
        {
            currentPiece.x++;
        }
        else
        {
            merge_piece();
            clear_lines();
            init_piece();
        }
    }
}

int main()
{
    srand(time(NULL));
    set_conio_terminal_mode();
    init_pieces();
    init_piece();

    int fd = oledOpen();

    while (1)
    {
        draw(fd);
        update();
        usleep(5000);
    }

    oledClose(fd);
    printf("Game Over! Your score is: %d\n", score);
    return 0;
}