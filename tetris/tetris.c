#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>

#define WIDTH 8
#define HEIGHT 32

typedef struct
{
    int x, y;
    int shape[4][4];
} Tetromino;

Tetromino currentPiece, pieces[7];
int board[HEIGHT][WIDTH] = {0};
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
                temp[x][3 - y] = currentPiece.shape[y][x];
            }
            else
            {
                temp[3 - x][y] = currentPiece.shape[y][x];
            }
        }
    }
    memcpy(currentPiece.shape, temp, sizeof(currentPiece.shape));
}

int check_collision(int x, int y)
{
    for (int dy = 0; dy < 4; dy++)
    {
        for (int dx = 0; dx < 4; dx++)
        {
            int nx = x + dx;
            int ny = y + dy;
            if (currentPiece.shape[dy][dx])
            {
                if (nx < 0 || nx >= WIDTH || ny >= HEIGHT || (ny >= 0 && board[ny][nx]))
                    return 1; // collision
            }
        }
    }
    return 0; // no collision
}

void merge_piece()
{
    for (int dy = 0; dy < 4; dy++)
    {
        for (int dx = 0; dx < 4; dx++)
        {
            if (currentPiece.shape[dy][dx] && currentPiece.y + dy >= 0)
            {
                board[currentPiece.y + dy][currentPiece.x + dx] = 1;
            }
        }
    }
}

void clear_lines()
{
    for (int y = HEIGHT - 1; y >= 0; y--)
    {
        int complete = 1;
        for (int x = 0; x < WIDTH; x++)
        {
            if (!board[y][x])
            {
                complete = 0;
                break;
            }
        }
        if (complete)
        {
            for (int ty = y; ty > 0; ty--)
            {
                for (int x = 0; x < WIDTH; x++)
                {
                    board[ty][x] = board[ty - 1][x];
                }
            }
            memset(board[0], 0, sizeof(board[0]));
            y++;
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
    currentPiece.x = WIDTH / 2 - 2;
    currentPiece.y = 0;
    // random shape for the new piece
    int r = rand() % 7;
    memcpy(currentPiece.shape, pieces[r].shape, sizeof(currentPiece.shape));
    if (check_collision(currentPiece.x, currentPiece.y))
    {
        printf("\nGame Over!\n");
        exit(0);
    }
}

void draw()
{
    printf("\033[H\033[J");
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            if (board[y][x])
            {
                printf("X");
            }
            else
            {
                int printed = 0;
                for (int dy = 0; dy < 4; dy++)
                {
                    for (int dx = 0; dx < 4; dx++)
                    {
                        if (currentPiece.shape[dy][dx] && y == currentPiece.y + dy && x == currentPiece.x + dx)
                        {
                            printf("X");
                            printed = 1;
                            break;
                        }
                    }
                    if (printed)
                        break;
                }
                if (!printed)
                    printf(" ");
            }
        }
        printf("\n");
    }
}

void update()
{
    if (kbhit())
    {
        char key = getch();
        switch (key)
        {
        case 'a': // left
            if (!check_collision(currentPiece.x - 1, currentPiece.y))
                currentPiece.x--;
            break;
        case 'd': // right
            if (!check_collision(currentPiece.x + 1, currentPiece.y))
                currentPiece.x++;
            break;
        case ' ': // rotate
            rotate_piece(1);
            if (check_collision(currentPiece.x, currentPiece.y))
                rotate_piece(0); // undo rotation if it results in a collision
            break;
        }
    }

    static int drop_counter = 0;
    drop_counter++;
    if (drop_counter >= 10)
    {
        drop_counter = 0;
        if (!check_collision(currentPiece.x, currentPiece.y + 1))
        {
            currentPiece.y++;
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

    while (1)
    {
        draw();
        update();
        usleep(50000);
    }

    return 0;
}
