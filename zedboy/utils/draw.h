#ifndef DRAW_H
#define DRAW_H

#define OLED_WIDTH 128
#define OLED_HEIGHT 32

extern unsigned char buf[];
extern int fd;


int clearScreen(int fd);

/* @brief draws pixel
   @param x pixel x coordinate
   @param y pixel y coordinate
   @param state 1 = on 0 = off */
void drawPixel(int x, int y, int state);

/* @brief draws line between 2 points
   @param startX start pixel x coordinate
   @param startY pixel y coordinate
   @param endX pixel x coordinate
   @param endY pixel y coordinate
   @param state 1 = on 0 = off*/
void drawLine(int startX, int startY, int endX, int endY, int state);

/* @brief draws empty rectangle between 2 points
   @param startX start pixel x coordinate
   @param startY pixel y coordinate
   @param endX pixel x coordinate
   @param endY pixel y coordinate
   @param state 1 = on 0 = off*/
void drawEmptyRectangle(int startX, int startY, int endX, int endY, int state);

/* @brief draws circle
   @param centerX x coordinate of center point
   @param centerY y coordinate of center point
   @param radius raiuds of desired circle */
void drawCircle(int centerX, int centerY, int radius);

/* @brief prints whole buffer in console for debugging */
void debugPrintBuffer();

/* @brief shows controls for each game
   @param fd oled file descriptor
   @param gameNum game # */
void showControls(int fd, int gameNum);

/* @brief shows controls for each game
   @param fd oled file descriptor
   @param gameNum game #
   @param win win or lose if applicable
   @param score final score achieved in game if applicable*/
void gameOver(int fd, int gameNum, int win, int score);

/* @brief draws string on OLED
   @param str string pointer
   @param origin_x origin x cooridnate
   @param origin_y origin y cordinate */
void drawWord(char *str, int origin_x, int origin_y);


/* @brief draws characer on OLED
   @param Char character to draw
   @param origin_x origin x cooridnate
   @param origin_y origin y cordinate */
void drawCharacter(char Character, int origin_x, int origin_y);


/* @brief Sends buf to OLED char device
   @param fd file to write to
   @param buf byte buffer pointer
   @return 0 on success, < 0 on error */
int sendBuffer(int fd, unsigned char *buf);

/* @brief Opens OLED char device
   @return 0 on success, < 0 on error */
int oledOpen();

/* @brief Closes OLED char device
   @return 0 on success, < 0 on error */
int oledClose(int fd);

#endif
