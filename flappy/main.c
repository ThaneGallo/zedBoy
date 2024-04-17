#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>

#define WIDTH 30
#define HEIGHT 20
#define GAP_SIZE 7
#define NUM_WALLS 3

int keyhit(void) {
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

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}

void display(char screen[HEIGHT][WIDTH], int bird_y, int gaps[NUM_WALLS], int wall_xs[NUM_WALLS], int score) {
    system("clear");
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            int wall_present = 0;
            for (int k = 0; k < NUM_WALLS; k++) {
                if (j == wall_xs[k] && (i < gaps[k] || i > gaps[k] + GAP_SIZE)) {
                    printf("#");
                    wall_present = 1;
                    break;
                }
            }
            if (i == bird_y && j == 2) {
                printf("@");
            } else if (!wall_present) {
                printf(" ");
            }
        }
        printf("\n");
    }
    printf("Score: %d\n", score); // Display score at the bottom of the screen
}

void game() {
    int bird_y = HEIGHT / 2;  // Start in the middle of the screen vertically
    int wall_xs[NUM_WALLS] = {WIDTH - 1, WIDTH - 1 + WIDTH / NUM_WALLS, WIDTH - 1 + 2 * WIDTH / NUM_WALLS};
    int gaps[NUM_WALLS];
    int score = 0;

    for (int i = 0; i < NUM_WALLS; i++) {
        gaps[i] = rand() % (HEIGHT - GAP_SIZE - 1) + 1;
    }

    char screen[HEIGHT][WIDTH];  // Declare and initialize the screen array
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            screen[i][j] = ' ';  // Initialize the screen with spaces
        }
    }

    while (1) {
        if (keyhit()) {
            char ch = getchar();
            if (ch == ' ') {
                bird_y -= 2;  // Flap: move the bird up by 2 positions
            }
        }

        bird_y++;  // Gravity effect: every loop iteration moves the bird down by 1 position

        if (bird_y < 0) bird_y = 0;  // Prevent flying above the screen
        if (bird_y >= HEIGHT) {
            printf("Game Over! Your score was: %d\n", score);
            break;  // Hit the ground
        }

        display(screen, bird_y, gaps, wall_xs, score);

        usleep(200000);  // Slow down the game loop

        for (int i = 0; i < NUM_WALLS; i++) {
            wall_xs[i]--;
            if (wall_xs[i] < 0) {
                wall_xs[i] = WIDTH - 1;
                gaps[i] = rand() % (HEIGHT - GAP_SIZE - 1) + 1;
                score++;  // Increment score when a wall goes off the screen and is reset
            }

            // Check for collisions with the walls
            if (wall_xs[i] == 2 && (bird_y < gaps[i] || bird_y > gaps[i] + GAP_SIZE)) {
                printf("Game Over! Your score was: %d\n", score);
                return;
            }
        }
    }
}

int main() {
    srand(time(NULL));  // Seed the random number generator
    game();
    return 0;
}
