#ifndef DRAW_H
#define DRAW_H

#define OLED_WIDTH 128
#define OLED_HEIGHT 32

extern unsigned char buf[];


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

/* @brief draws string on OLED
   @param str string pointer
   @param row picks which number to write text to */
void drawWord(char *str, int row);


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
