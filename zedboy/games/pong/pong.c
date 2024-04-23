#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>

#define WIDTH 40
#define HEIGHT 10

int ballX = WIDTH / 2, ballY = HEIGHT / 2;
int velocityX = -1, velocityY = 1;
int playerPaddleY = HEIGHT / 2;
int AIPaddleY = HEIGHT / 2;
int playerScore = 0; // Player's score
int aiScore = 0; // AI score
char field[HEIGHT][WIDTH + 1]; // +1 for null terminator

void initializeField() {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            field[y][x] = '.';
        }
        field[y][WIDTH] = '\0';
    }
}

void draw() {
    system("clear"); // Use "cls" on Windows
    for (int y = 0; y < HEIGHT; y++) {
        printf("%s\n", field[y]);
    }
    printf("\nScore: %d - %d\n", playerScore, aiScore);
    printf("Use 'w' and 's' to move up and down\n");
}

void updateBall() {
    ballX += velocityX;
    ballY += velocityY;

    // Ball collision with top or bottom
    if (ballY <= 0 || ballY >= HEIGHT - 1) {
        velocityY = -velocityY;
    }

    // Ball collision with the paddles
    if (ballX == 2 && (ballY >= playerPaddleY && ballY < playerPaddleY + 4)) {
        velocityX = -velocityX;
    } else if (ballX == WIDTH - 3 && (ballY >= AIPaddleY && ballY < AIPaddleY + 4)) {
        velocityX = -velocityX;
    } else if (ballX <= 1) {
        // Ball is out of bounds, reset position
        aiScore++;
        ballX = WIDTH / 2;
        ballY = HEIGHT / 2;
        velocityX = -velocityX; // Send ball in opposite direction
    } else if (ballX >= WIDTH - 2) {
        // Player scores if the ball gets past the AI paddle
        playerScore++;
        ballX = WIDTH / 2; ballY = HEIGHT / 2; // Reset ball position
        velocityX = -velocityX; // Send ball in opposite direction
    }
}

void moveAI() {
    if (AIPaddleY < ballY - 1) {
        AIPaddleY++;
    } else if (AIPaddleY > ballY + 1) {
        AIPaddleY--;
    }

    // Prevent the AI paddle from moving out of bounds
    if (AIPaddleY < 0) {
        AIPaddleY = 0;
    } else if (AIPaddleY > HEIGHT - 4) {
        AIPaddleY = HEIGHT - 4;
    }
}

void placeEntities() {
    initializeField();
    // Place player paddle
    for (int i = 0; i < 4; ++i) {
        field[playerPaddleY + i][1] = '|';
    }
    // Place AI paddle
    for (int i = 0; i < 4; ++i) {
        field[AIPaddleY + i][WIDTH - 2] = '|';
    }
    // Place ball
    field[ballY][ballX] = 'O';
}

int keyhit(void) {
    struct termios oldt, newt;
    int ch;
    int oldf;

    // Save old terminal settings
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;

    // Disable buffering and echo
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    // Set non-blocking mode
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    // Check for input
    ch = getchar();

    // Restore terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

void input() {
    if (keyhit()) {
        char ch = getchar();
        switch (ch) {
            case 'w':
                if (playerPaddleY > 0) playerPaddleY--;
                break;
            case 's':
                if (playerPaddleY < HEIGHT - 4) playerPaddleY++;
                break;
        }
    }
}

int main() {
    // Game setup
    srand(time(NULL));
    initializeField();
    placeEntities();

    // Main game loop
    while (1) {
        placeEntities();
        draw();
        input();
        updateBall();
        moveAI();
        usleep(100000); // Sleep for 0.1 seconds to slow down the game loop
    }

    return 0;
}
