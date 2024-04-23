#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <sys/mman.h>

#include "draw.h"

unsigned char buf[(OLED_HEIGHT * OLED_WIDTH) / 8];
int fd;

/* @brief draws pixel
   @param x pixel x coordinate
   @param y pixel y coordinate
   @param state 1 = on 0 = off
   @return 0 on success, < 0 on error */
int drawPixel(int x, int y, int state)
{
    int byte_offset;
    int pgNum;

    // page = 1 byte
    pgNum = y / 8;

    if ((x > OLED_WIDTH || x < 0) || (y > OLED_HEIGHT || y < 0))
    {
        printf("drawPixel out of bounds( x%d,y%d)\n");
        return -1;
    }

    // 4 pages per column  + pg offset
    byte_offset = x * (OLED_HEIGHT / 8) + pgNum;

    // Adjust pixel state in buf based on the provided coordinates
    if (state == 1)
    {
        // Set the corresponding bit in the byte
        buf[byte_offset] |= (1 << (y % 8)); // Set the bit at position y % 8
    }
    else
    {
        // Clear the corresponding bit in the byte
        buf[byte_offset] &= ~(1 << (y % 8)); // Clear the bit at position y % 8
    }

    return 0;
}

/* @brief draws line between 2 points
   @param startX start pixel x coordinate
   @param startY pixel y coordinate
   @param endX pixel x coordinate
   @param endY pixel y coordinate
   @param state 1 = on 0 = off */
void drawLine(int startX, int startY, int endX, int endY, int state)
{
    int dx, dy;

    int i, j = 0;

    dx = endX - startX;
    dy = endY - startY;

    // pixel has no length and draws itself
    if ((dy == 0) && (dx == 0))
    {
        drawPixel(startX, startY, state);
    }

    // draws lines left / right
    else if (dy == 0)
    {
        // right
        if (dx > 0)
        {
            for (i = 0; i < dx; i++)
            {
                drawPixel(startX + i, startY, state);
            }
        }
        // left
        else if (dx < 0)
        {
            for (i = 0; i + 1 > dx; i--)
            {
                drawPixel(startX + i, startY, state);
            }
        }
    }

    // draws lines up / down
    else if (dx == 0)
    {
        // up
        if (dy > 0)
        {
            for (j = 0; j < dy; j++)
            {
                drawPixel(startX, startY + j, state);
            }
        }
        // down
        else if (dy < 0)
        {
            for (j = 0; j + 1 > dy; j--)
            {
                drawPixel(startX, startY + j, state);
            }
        }
    }

    // diagonal lines
    // else
    // {
    //     if ((dx > 0) && (dy > 0))
    //     {

    //     }

    //     else if ((dx < 0) && (dy > 0))
    //     {

    //     }

    //     else if ((dx > 0) && (dy < 0))
    //     {

    //     }

    //     else
    //     {

    //     }
    // }
}

/* @brief draws empty rectangle between 2 points
   @param startX start pixel x coordinate
   @param startY pixel y coordinate
   @param endX pixel x coordinate
   @param endY pixel y coordinate
   @param state 1 = on 0 = off */
void drawEmptyRectangle(int startX, int startY, int endX, int endY, int state)
{

    drawLine(startX, startY, startX, endY, state);
    drawLine(startX, startY, endX, startY, state);
    drawLine(endX, startY, endX, endY, state);
    drawLine(startX, endY, endX, endY, state);
}

/* @brief draws circle
   @param centerX x coordinate of center point
   @param centerY y coordinate of center point
   @param radius raiuds of desired circle
   @return 0 on success, < 0 on error */
void drawCircle(int centerX, int centerY, int radius)
{
    if (radius > 0 && radius < 5)
    {
        switch (radius)
        {
        case 1:
            drawPixel(centerX, centerY, 1);
            break;
        case 2:
            drawPixel(centerX, centerY, 1);
            drawPixel(centerX + 1, centerY, 1);
            drawPixel(centerX - 1, centerY, 1);
            drawPixel(centerX, centerY + 1, 1);
            drawPixel(centerX, centerY - 1, 1);
            break;
        case 3:
            drawEmptyRectangle(centerX - 1, centerY - 1, centerX + 1, centerY + 1, 1);
            break;
        case 4:
            drawPixel(centerX, centerY, 1);
            drawPixel(centerX + 1, centerY, 1);
            drawPixel(centerX - 1, centerY + 1, 1);
            drawPixel(centerX - 1, centerY + 2, 1);
            drawPixel(centerX + 2, centerY - 1, 1);
            drawPixel(centerX + 2, centerY - 2, 1);
            drawPixel(centerX, centerY - 3, 1);
            drawPixel(centerX + 1, centerY - 3, 1);
            break;
        }
    }
}

void debugPrintBuffer()
{
    unsigned int i, j;

    // for checking buffer contents
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 128; j++)
        {
            printf("%x", buf[j * 4 + i]);
        }
        printf("\n");
    }
}

/* @brief draws characer on OLED
   @param Char character to draw
   @param origin_x origin x cooridnate
   @param origin_y origin y cordinate */
void drawCharacter(char Character, int origin_x, int origin_y)
{
    // every Char is 4 wide and 5 tall
    switch (Character)
    {

    case ' ':
        break;

    case 'a':
        drawPixel(1 + origin_x, 0 + origin_y, 1);
        drawPixel(2 + origin_x, 0 + origin_y, 1);

        drawPixel(0 + origin_x, 1 + origin_y, 1);
        drawPixel(3 + origin_x, 1 + origin_y, 1);

        drawPixel(0 + origin_x, 2 + origin_y, 1);
        drawPixel(1 + origin_x, 2 + origin_y, 1);
        drawPixel(2 + origin_x, 2 + origin_y, 1);
        drawPixel(3 + origin_x, 2 + origin_y, 1);

        drawPixel(0 + origin_x, 3 + origin_y, 1);
        drawPixel(3 + origin_x, 3 + origin_y, 1);

        drawPixel(0 + origin_x, 4 + origin_y, 1);
        drawPixel(3 + origin_x, 4 + origin_y, 1);
        break;

    case 'b':
        drawPixel(0 + origin_x, 0 + origin_y, 1);
        drawPixel(1 + origin_x, 0 + origin_y, 1);
        drawPixel(2 + origin_x, 0 + origin_y, 1);

        drawPixel(0 + origin_x, 1 + origin_y, 1);
        drawPixel(3 + origin_x, 1 + origin_y, 1);

        drawPixel(0 + origin_x, 2 + origin_y, 1);
        drawPixel(1 + origin_x, 2 + origin_y, 1);
        drawPixel(2 + origin_x, 2 + origin_y, 1);

        drawPixel(0 + origin_x, 3 + origin_y, 1);
        drawPixel(3 + origin_x, 3 + origin_y, 1);

        drawPixel(0 + origin_x, 4 + origin_y, 1);
        drawPixel(1 + origin_x, 4 + origin_y, 1);
        drawPixel(2 + origin_x, 4 + origin_y, 1);

        break;

    case 'c':
        drawPixel(1 + origin_x, 0 + origin_y, 1);
        drawPixel(2 + origin_x, 0 + origin_y, 1);
        drawPixel(3 + origin_x, 0 + origin_y, 1);

        drawPixel(0 + origin_x, 1 + origin_y, 1);
        drawPixel(0 + origin_x, 2 + origin_y, 1);
        drawPixel(0 + origin_x, 3 + origin_y, 1);

        drawPixel(1 + origin_x, 4 + origin_y, 1);
        drawPixel(2 + origin_x, 4 + origin_y, 1);
        drawPixel(3 + origin_x, 4 + origin_y, 1);

        break;

    case 'd':

        drawPixel(0 + origin_x, 0 + origin_y, 1);
        drawPixel(1 + origin_x, 0 + origin_y, 1);
        drawPixel(2 + origin_x, 0 + origin_y, 1);

        drawPixel(0 + origin_x, 1 + origin_y, 1);
        drawPixel(3 + origin_x, 1 + origin_y, 1);

        drawPixel(0 + origin_x, 2 + origin_y, 1);
        drawPixel(3 + origin_x, 2 + origin_y, 1);

        drawPixel(0 + origin_x, 3 + origin_y, 1);
        drawPixel(3 + origin_x, 3 + origin_y, 1);

        drawPixel(0 + origin_x, 4 + origin_y, 1);
        drawPixel(1 + origin_x, 4 + origin_y, 1);
        drawPixel(2 + origin_x, 4 + origin_y, 1);

        break;

    case 'e':
        drawPixel(0 + origin_x, 0 + origin_y, 1);
        drawPixel(1 + origin_x, 0 + origin_y, 1);
        drawPixel(2 + origin_x, 0 + origin_y, 1);
        drawPixel(3 + origin_x, 0 + origin_y, 1);

        drawPixel(0 + origin_x, 1 + origin_y, 1);

        drawPixel(0 + origin_x, 2 + origin_y, 1);
        drawPixel(1 + origin_x, 2 + origin_y, 1);
        drawPixel(2 + origin_x, 2 + origin_y, 1);

        drawPixel(0 + origin_x, 3 + origin_y, 1);

        drawPixel(0 + origin_x, 4 + origin_y, 1);
        drawPixel(1 + origin_x, 4 + origin_y, 1);
        drawPixel(2 + origin_x, 4 + origin_y, 1);
        drawPixel(3 + origin_x, 4 + origin_y, 1);

        break;

    case 'f':
        drawPixel(0 + origin_x, 0 + origin_y, 1);
        drawPixel(1 + origin_x, 0 + origin_y, 1);
        drawPixel(2 + origin_x, 0 + origin_y, 1);
        drawPixel(3 + origin_x, 0 + origin_y, 1);

        drawPixel(0 + origin_x, 1 + origin_y, 1);

        drawPixel(0 + origin_x, 2 + origin_y, 1);
        drawPixel(1 + origin_x, 2 + origin_y, 1);
        drawPixel(2 + origin_x, 2 + origin_y, 1);

        drawPixel(0 + origin_x, 3 + origin_y, 1);

        drawPixel(0 + origin_x, 4 + origin_y, 1);

        break;

    case 'g':
        drawPixel(1 + origin_x, 0 + origin_y, 1);
        drawPixel(2 + origin_x, 0 + origin_y, 1);
        drawPixel(3 + origin_x, 0 + origin_y, 1);

        drawPixel(0 + origin_x, 1 + origin_y, 1);

        drawPixel(0 + origin_x, 2 + origin_y, 1);
        drawPixel(2 + origin_x, 2 + origin_y, 1);
        drawPixel(3 + origin_x, 2 + origin_y, 1);

        drawPixel(0 + origin_x, 3 + origin_y, 1);
        drawPixel(3 + origin_x, 3 + origin_y, 1);

        drawPixel(1 + origin_x, 4 + origin_y, 1);
        drawPixel(2 + origin_x, 4 + origin_y, 1);
        drawPixel(3 + origin_x, 4 + origin_y, 1);

        break;

    case 'h':
        drawPixel(0 + origin_x, 0 + origin_y, 1);
        drawPixel(3 + origin_x, 0 + origin_y, 1);

        drawPixel(0 + origin_x, 1 + origin_y, 1);
        drawPixel(3 + origin_x, 1 + origin_y, 1);

        drawPixel(0 + origin_x, 2 + origin_y, 1);
        drawPixel(1 + origin_x, 2 + origin_y, 1);
        drawPixel(2 + origin_x, 2 + origin_y, 1);
        drawPixel(3 + origin_x, 2 + origin_y, 1);

        drawPixel(0 + origin_x, 3 + origin_y, 1);
        drawPixel(3 + origin_x, 3 + origin_y, 1);

        drawPixel(0 + origin_x, 4 + origin_y, 1);
        drawPixel(3 + origin_x, 4 + origin_y, 1);

        break;

    case 'i':
        drawPixel(1 + origin_x, 0 + origin_y, 1);
        drawPixel(1 + origin_x, 1 + origin_y, 1);
        drawPixel(1 + origin_x, 2 + origin_y, 1);
        drawPixel(1 + origin_x, 3 + origin_y, 1);
        drawPixel(1 + origin_x, 4 + origin_y, 1);

        break;

    case 'j':
        drawPixel(3 + origin_x, 0 + origin_y, 1);

        drawPixel(3 + origin_x, 1 + origin_y, 1);

        drawPixel(3 + origin_x, 2 + origin_y, 1);

        drawPixel(0 + origin_x, 3 + origin_y, 1);
        drawPixel(3 + origin_x, 3 + origin_y, 1);

        drawPixel(1 + origin_x, 4 + origin_y, 1);
        drawPixel(2 + origin_x, 4 + origin_y, 1);

        break;

    case 'k':
        drawPixel(0 + origin_x, 0 + origin_y, 1);
        drawPixel(3 + origin_x, 0 + origin_y, 1);

        drawPixel(0 + origin_x, 1 + origin_y, 1);
        drawPixel(2 + origin_x, 1 + origin_y, 1);

        drawPixel(0 + origin_x, 2 + origin_y, 1);
        drawPixel(1 + origin_x, 2 + origin_y, 1);

        drawPixel(0 + origin_x, 3 + origin_y, 1);
        drawPixel(2 + origin_x, 3 + origin_y, 1);

        drawPixel(0 + origin_x, 4 + origin_y, 1);
        drawPixel(3 + origin_x, 4 + origin_y, 1);

        break;

    case 'l':
        drawPixel(0 + origin_x, 0 + origin_y, 1);
        drawPixel(0 + origin_x, 1 + origin_y, 1);
        drawPixel(0 + origin_x, 2 + origin_y, 1);
        drawPixel(0 + origin_x, 3 + origin_y, 1);

        drawPixel(0 + origin_x, 4 + origin_y, 1);
        drawPixel(1 + origin_x, 4 + origin_y, 1);
        drawPixel(2 + origin_x, 4 + origin_y, 1);
        break;

    case 'm':
        drawPixel(0 + origin_x, 0 + origin_y, 1);
        drawPixel(4 + origin_x, 0 + origin_y, 1);

        drawPixel(0 + origin_x, 1 + origin_y, 1);
        drawPixel(1 + origin_x, 1 + origin_y, 1);
        drawPixel(3 + origin_x, 1 + origin_y, 1);
        drawPixel(4 + origin_x, 1 + origin_y, 1);

        drawPixel(0 + origin_x, 2 + origin_y, 1);
        drawPixel(2 + origin_x, 2 + origin_y, 1);
        drawPixel(4 + origin_x, 2 + origin_y, 1);

        drawPixel(0 + origin_x, 3 + origin_y, 1);
        drawPixel(4 + origin_x, 3 + origin_y, 1);

        drawPixel(0 + origin_x, 4 + origin_y, 1);
        drawPixel(4 + origin_x, 4 + origin_y, 1);
        break;

    case 'n':
        drawPixel(0 + origin_x, 0 + origin_y, 1);
        drawPixel(3 + origin_x, 0 + origin_y, 1);

        drawPixel(0 + origin_x, 1 + origin_y, 1);
        drawPixel(1 + origin_x, 1 + origin_y, 1);
        drawPixel(3 + origin_x, 1 + origin_y, 1);

        drawPixel(0 + origin_x, 2 + origin_y, 1);
        drawPixel(2 + origin_x, 2 + origin_y, 1);
        drawPixel(3 + origin_x, 2 + origin_y, 1);

        drawPixel(0 + origin_x, 3 + origin_y, 1);
        drawPixel(3 + origin_x, 3 + origin_y, 1);

        drawPixel(0 + origin_x, 4 + origin_y, 1);
        drawPixel(3 + origin_x, 4 + origin_y, 1);
        break;

    case 'o':
        drawPixel(1 + origin_x, 0 + origin_y, 1);
        drawPixel(2 + origin_x, 0 + origin_y, 1);

        drawPixel(0 + origin_x, 1 + origin_y, 1);
        drawPixel(3 + origin_x, 1 + origin_y, 1);

        drawPixel(0 + origin_x, 2 + origin_y, 1);
        drawPixel(3 + origin_x, 2 + origin_y, 1);

        drawPixel(0 + origin_x, 3 + origin_y, 1);
        drawPixel(3 + origin_x, 3 + origin_y, 1);

        drawPixel(1 + origin_x, 4 + origin_y, 1);
        drawPixel(2 + origin_x, 4 + origin_y, 1);

        break;

    case 'p':

        drawPixel(0 + origin_x, 0 + origin_y, 1);
        drawPixel(1 + origin_x, 0 + origin_y, 1);
        drawPixel(2 + origin_x, 0 + origin_y, 1);

        drawPixel(0 + origin_x, 1 + origin_y, 1);
        drawPixel(3 + origin_x, 1 + origin_y, 1);

        drawPixel(0 + origin_x, 2 + origin_y, 1);
        drawPixel(1 + origin_x, 2 + origin_y, 1);
        drawPixel(2 + origin_x, 2 + origin_y, 1);

        drawPixel(0 + origin_x, 3 + origin_y, 1);

        drawPixel(0 + origin_x, 4 + origin_y, 1);

        break;

    case 'q':
        drawPixel(1 + origin_x, 0 + origin_y, 1);
        drawPixel(2 + origin_x, 0 + origin_y, 1);

        drawPixel(0 + origin_x, 1 + origin_y, 1);
        drawPixel(3 + origin_x, 1 + origin_y, 1);

        drawPixel(0 + origin_x, 2 + origin_y, 1);
        drawPixel(3 + origin_x, 2 + origin_y, 1);

        drawPixel(0 + origin_x, 3 + origin_y, 1);
        drawPixel(2 + origin_x, 3 + origin_y, 1);
        drawPixel(3 + origin_x, 3 + origin_y, 1);

        drawPixel(1 + origin_x, 4 + origin_y, 1);
        drawPixel(2 + origin_x, 4 + origin_y, 1);
        drawPixel(3 + origin_x, 4 + origin_y, 1);

        break;

    case 'r':
        drawPixel(0 + origin_x, 0 + origin_y, 1);
        drawPixel(1 + origin_x, 0 + origin_y, 1);
        drawPixel(2 + origin_x, 0 + origin_y, 1);

        drawPixel(0 + origin_x, 1 + origin_y, 1);
        drawPixel(3 + origin_x, 1 + origin_y, 1);

        drawPixel(0 + origin_x, 2 + origin_y, 1);
        drawPixel(1 + origin_x, 2 + origin_y, 1);
        drawPixel(2 + origin_x, 2 + origin_y, 1);

        drawPixel(0 + origin_x, 3 + origin_y, 1);
        drawPixel(2 + origin_x, 3 + origin_y, 1);

        drawPixel(0 + origin_x, 4 + origin_y, 1);
        drawPixel(3 + origin_x, 4 + origin_y, 1);

        break;

    case 's':
        drawPixel(1 + origin_x, 0 + origin_y, 1);
        drawPixel(2 + origin_x, 0 + origin_y, 1);
        drawPixel(3 + origin_x, 0 + origin_y, 1);

        drawPixel(0 + origin_x, 1 + origin_y, 1);

        drawPixel(1 + origin_x, 2 + origin_y, 1);
        drawPixel(2 + origin_x, 2 + origin_y, 1);

        drawPixel(3 + origin_x, 3 + origin_y, 1);

        drawPixel(0 + origin_x, 4 + origin_y, 1);
        drawPixel(1 + origin_x, 4 + origin_y, 1);
        drawPixel(2 + origin_x, 4 + origin_y, 1);

        break;

    case 't':
        drawPixel(0 + origin_x, 0 + origin_y, 1);
        drawPixel(1 + origin_x, 0 + origin_y, 1);
        drawPixel(2 + origin_x, 0 + origin_y, 1);

        drawPixel(1 + origin_x, 1 + origin_y, 1);

        drawPixel(1 + origin_x, 2 + origin_y, 1);

        drawPixel(1 + origin_x, 3 + origin_y, 1);

        drawPixel(1 + origin_x, 4 + origin_y, 1);

        break;

    case 'u':
        drawPixel(0 + origin_x, 0 + origin_y, 1);
        drawPixel(3 + origin_x, 0 + origin_y, 1);

        drawPixel(0 + origin_x, 1 + origin_y, 1);
        drawPixel(3 + origin_x, 1 + origin_y, 1);

        drawPixel(0 + origin_x, 2 + origin_y, 1);
        drawPixel(3 + origin_x, 2 + origin_y, 1);

        drawPixel(0 + origin_x, 3 + origin_y, 1);
        drawPixel(3 + origin_x, 3 + origin_y, 1);

        drawPixel(0 + origin_x, 4 + origin_y, 1);
        drawPixel(1 + origin_x, 4 + origin_y, 1);
        drawPixel(2 + origin_x, 4 + origin_y, 1);
        drawPixel(3 + origin_x, 4 + origin_y, 1);

        break;

    case 'v':
        drawPixel(0 + origin_x, 0 + origin_y, 1);
        drawPixel(2 + origin_x, 0 + origin_y, 1);

        drawPixel(0 + origin_x, 1 + origin_y, 1);
        drawPixel(2 + origin_x, 1 + origin_y, 1);

        drawPixel(0 + origin_x, 2 + origin_y, 1);
        drawPixel(2 + origin_x, 2 + origin_y, 1);

        drawPixel(0 + origin_x, 3 + origin_y, 1);
        drawPixel(2 + origin_x, 3 + origin_y, 1);

        drawPixel(1 + origin_x, 4 + origin_y, 1);

        break;

    case 'w':
        drawPixel(0 + origin_x, 0 + origin_y, 1);
        drawPixel(4 + origin_x, 0 + origin_y, 1);

        drawPixel(0 + origin_x, 1 + origin_y, 1);
        drawPixel(4 + origin_x, 1 + origin_y, 1);

        drawPixel(0 + origin_x, 2 + origin_y, 1);
        drawPixel(4 + origin_x, 2 + origin_y, 1);

        drawPixel(0 + origin_x, 3 + origin_y, 1);
        drawPixel(2 + origin_x, 3 + origin_y, 1);
        drawPixel(4 + origin_x, 3 + origin_y, 1);

        drawPixel(1 + origin_x, 4 + origin_y, 1);

        break;

    case 'x':
        drawPixel(0 + origin_x, 0 + origin_y, 1);
        drawPixel(2 + origin_x, 0 + origin_y, 1);

        drawPixel(0 + origin_x, 1 + origin_y, 1);
        drawPixel(2 + origin_x, 1 + origin_y, 1);

        drawPixel(1 + origin_x, 2 + origin_y, 1);

        drawPixel(0 + origin_x, 3 + origin_y, 1);
        drawPixel(2 + origin_x, 3 + origin_y, 1);

        drawPixel(0 + origin_x, 4 + origin_y, 1);
        drawPixel(2 + origin_x, 4 + origin_y, 1);

        break;

    case 'y':
        drawPixel(0 + origin_x, 0 + origin_y, 1);
        drawPixel(2 + origin_x, 0 + origin_y, 1);

        drawPixel(0 + origin_x, 1 + origin_y, 1);
        drawPixel(2 + origin_x, 1 + origin_y, 1);

        drawPixel(0 + origin_x, 2 + origin_y, 1);
        drawPixel(1 + origin_x, 2 + origin_y, 1);
        drawPixel(2 + origin_x, 2 + origin_y, 1);

        drawPixel(1 + origin_x, 3 + origin_y, 1);

        drawPixel(1 + origin_x, 4 + origin_y, 1);

        break;

    case 'z':
        drawPixel(0 + origin_x, 0 + origin_y, 1);
        drawPixel(1 + origin_x, 0 + origin_y, 1);
        drawPixel(2 + origin_x, 0 + origin_y, 1);
        drawPixel(3 + origin_x, 0 + origin_y, 1);

        drawPixel(3 + origin_x, 1 + origin_y, 1);

        drawPixel(2 + origin_x, 2 + origin_y, 1);

        drawPixel(1 + origin_x, 3 + origin_y, 1);

        drawPixel(0 + origin_x, 4 + origin_y, 1);
        drawPixel(1 + origin_x, 4 + origin_y, 1);
        drawPixel(2 + origin_x, 4 + origin_y, 1);
        drawPixel(3 + origin_x, 4 + origin_y, 1);
        break;

    case ':':
        drawPixel(0 + origin_x, 0 + origin_y, 1);
        drawPixel(1 + origin_x, 0 + origin_y, 1);

        drawPixel(0 + origin_x, 1 + origin_y, 1);
        drawPixel(1 + origin_x, 1 + origin_y, 1);

        drawPixel(0 + origin_x, 3 + origin_y, 1);
        drawPixel(1 + origin_x, 3 + origin_y, 1);

        drawPixel(0 + origin_x, 4 + origin_y, 1);
        drawPixel(1 + origin_x, 4 + origin_y, 1);

        break;

    case '0':
        drawPixel(origin_x, origin_y, 1);
        drawPixel(origin_x + 1, origin_y, 1);
        drawPixel(origin_x + 2, origin_y, 1);
        drawPixel(origin_x + 3, origin_y, 1);
        drawPixel(origin_x, origin_y + 4, 1);
        drawPixel(origin_x + 1, origin_y + 4, 1);
        drawPixel(origin_x + 2, origin_y + 4, 1);
        drawPixel(origin_x + 3, origin_y + 4, 1);
        drawPixel(origin_x, origin_y + 1, 1);
        drawPixel(origin_x, origin_y + 2, 1);
        drawPixel(origin_x, origin_y + 3, 1);
        drawPixel(origin_x + 3, origin_y + 1, 1);
        drawPixel(origin_x + 3, origin_y + 2, 1);
        drawPixel(origin_x + 3, origin_y + 3, 1);
        break;

    case '1':
        drawPixel(origin_x + 1, origin_y, 1);
        drawPixel(origin_x + 2, origin_y, 1);
        drawPixel(origin_x + 2, origin_y + 1, 1);
        drawPixel(origin_x + 2, origin_y + 2, 1);
        drawPixel(origin_x + 2, origin_y + 3, 1);
        drawPixel(origin_x + 2, origin_y + 4, 1);
        break;

    case '2':
        drawPixel(origin_x, origin_y, 1);
        drawPixel(origin_x + 1, origin_y, 1);
        drawPixel(origin_x + 2, origin_y, 1);
        drawPixel(origin_x + 3, origin_y, 1);
        drawPixel(origin_x + 3, origin_y + 1, 1);
        drawPixel(origin_x + 2, origin_y + 2, 1);
        drawPixel(origin_x + 1, origin_y + 3, 1);
        drawPixel(origin_x, origin_y + 4, 1);
        drawPixel(origin_x + 1, origin_y + 4, 1);
        drawPixel(origin_x + 2, origin_y + 4, 1);
        drawPixel(origin_x + 3, origin_y + 4, 1);
        break;

    case '3':
        drawPixel(origin_x, origin_y, 1);
        drawPixel(origin_x + 1, origin_y, 1);
        drawPixel(origin_x + 2, origin_y, 1);
        drawPixel(origin_x + 3, origin_y, 1);
        drawPixel(origin_x + 3, origin_y + 1, 1);
        drawPixel(origin_x + 2, origin_y + 2, 1);
        drawPixel(origin_x + 3, origin_y + 2, 1);
        drawPixel(origin_x + 3, origin_y + 3, 1);
        drawPixel(origin_x + 2, origin_y + 4, 1);
        drawPixel(origin_x + 1, origin_y + 4, 1);
        drawPixel(origin_x, origin_y + 4, 1);
        break;

    case '4':
        drawPixel(origin_x, origin_y, 1);
        drawPixel(origin_x, origin_y + 1, 1);
        drawPixel(origin_x, origin_y + 2, 1);
        drawPixel(origin_x + 1, origin_y + 2, 1);
        drawPixel(origin_x + 2, origin_y + 2, 1);
        drawPixel(origin_x + 2, origin_y, 1);
        drawPixel(origin_x + 2, origin_y + 1, 1);
        drawPixel(origin_x + 2, origin_y + 3, 1);
        drawPixel(origin_x + 2, origin_y + 4, 1);
        break;

    case '5':
        drawPixel(origin_x + 3, origin_y, 1);
        drawPixel(origin_x + 2, origin_y, 1);
        drawPixel(origin_x + 1, origin_y, 1);
        drawPixel(origin_x, origin_y, 1);
        drawPixel(origin_x, origin_y + 1, 1);
        drawPixel(origin_x, origin_y + 2, 1);
        drawPixel(origin_x + 1, origin_y + 2, 1);
        drawPixel(origin_x + 2, origin_y + 2, 1);
        drawPixel(origin_x + 3, origin_y + 2, 1);
        drawPixel(origin_x + 3, origin_y + 3, 1);
        drawPixel(origin_x + 2, origin_y + 4, 1);
        drawPixel(origin_x + 1, origin_y + 4, 1);
        drawPixel(origin_x, origin_y + 4, 1);
        break;

    case '6':
        drawPixel(origin_x + 3, origin_y, 1);
        drawPixel(origin_x + 2, origin_y, 1);
        drawPixel(origin_x + 1, origin_y, 1);
        drawPixel(origin_x, origin_y, 1);
        drawPixel(origin_x, origin_y + 1, 1);
        drawPixel(origin_x, origin_y + 2, 1);
        drawPixel(origin_x + 1, origin_y + 2, 1);
        drawPixel(origin_x + 2, origin_y + 2, 1);
        drawPixel(origin_x + 3, origin_y + 2, 1);
        drawPixel(origin_x + 3, origin_y + 1, 1);
        drawPixel(origin_x + 2, origin_y + 4, 1);
        drawPixel(origin_x + 1, origin_y + 4, 1);
        drawPixel(origin_x, origin_y + 4, 1);
        break;

    case '7':
        drawPixel(origin_x, origin_y, 1);
        drawPixel(origin_x + 1, origin_y, 1);
        drawPixel(origin_x + 2, origin_y, 1);
        drawPixel(origin_x + 3, origin_y, 1);
        drawPixel(origin_x + 3, origin_y + 1, 1);
        drawPixel(origin_x + 2, origin_y + 2, 1);
        drawPixel(origin_x + 1, origin_y + 3, 1);
        drawPixel(origin_x, origin_y + 4, 1);
        break;

    case '8':
        drawPixel(origin_x, origin_y, 1);
        drawPixel(origin_x + 1, origin_y, 1);
        drawPixel(origin_x + 2, origin_y, 1);
        drawPixel(origin_x + 3, origin_y, 1);
        drawPixel(origin_x, origin_y + 1, 1);
        drawPixel(origin_x + 3, origin_y + 1, 1);
        drawPixel(origin_x, origin_y + 2, 1);
        drawPixel(origin_x + 1, origin_y + 2, 1);
        drawPixel(origin_x + 2, origin_y + 2, 1);
        drawPixel(origin_x + 3, origin_y + 2, 1);
        drawPixel(origin_x, origin_y + 3, 1);
        drawPixel(origin_x + 3, origin_y + 3, 1);
        drawPixel(origin_x, origin_y + 4, 1);
        drawPixel(origin_x + 1, origin_y + 4, 1);
        drawPixel(origin_x + 2, origin_y + 4, 1);
        drawPixel(origin_x + 3, origin_y + 4, 1);
        break;

    case '9':
        drawPixel(origin_x, origin_y, 1);
        drawPixel(origin_x + 1, origin_y, 1);
        drawPixel(origin_x + 2, origin_y, 1);
        drawPixel(origin_x + 3, origin_y, 1);
        drawPixel(origin_x, origin_y + 1, 1);
        drawPixel(origin_x + 3, origin_y + 1, 1);
        drawPixel(origin_x, origin_y + 2, 1);
        drawPixel(origin_x + 1, origin_y + 2, 1);
        drawPixel(origin_x + 2, origin_y + 2, 1);
        drawPixel(origin_x + 3, origin_y + 2, 1);
        drawPixel(origin_x + 3, origin_y + 3, 1);
        drawPixel(origin_x, origin_y + 4, 1);
        drawPixel(origin_x + 1, origin_y + 4, 1);
        drawPixel(origin_x + 2, origin_y + 4, 1);
        drawPixel(origin_x + 3, origin_y + 4, 1);
        break;

    default:

        drawPixel(0 + origin_x, 0 + origin_y, 1);
        drawPixel(1 + origin_x, 0 + origin_y, 1);
        drawPixel(2 + origin_x, 0 + origin_y, 1);
        drawPixel(3 + origin_x, 0 + origin_y, 1);

        drawPixel(0 + origin_x, 1 + origin_y, 1);
        drawPixel(1 + origin_x, 1 + origin_y, 1);
        drawPixel(2 + origin_x, 1 + origin_y, 1);
        drawPixel(3 + origin_x, 1 + origin_y, 1);

        drawPixel(0 + origin_x, 2 + origin_y, 1);
        drawPixel(1 + origin_x, 2 + origin_y, 1);
        drawPixel(2 + origin_x, 2 + origin_y, 1);
        drawPixel(3 + origin_x, 2 + origin_y, 1);

        drawPixel(0 + origin_x, 3 + origin_y, 1);
        drawPixel(1 + origin_x, 3 + origin_y, 1);
        drawPixel(2 + origin_x, 3 + origin_y, 1);
        drawPixel(3 + origin_x, 3 + origin_y, 1);

        drawPixel(0 + origin_x, 4 + origin_y, 1);
        drawPixel(1 + origin_x, 4 + origin_y, 1);
        drawPixel(2 + origin_x, 4 + origin_y, 1);
        drawPixel(3 + origin_x, 4 + origin_y, 1);
    }
}

/* @brief draws string on OLED
   @param str string pointer
   @param origin_x origin x cooridnate
   @param origin_y origin y cordinate */
void drawWord(char *str, int origin_x, int origin_y)
{
    int i = 0;

    while (str[i] != '\0')
    {
        drawCharacter(str[i], origin_x, origin_y);
        i++;
        origin_x += 5;
    }
}

/* @brief shows controls for each game
   @param fd oled file descriptor
   @param gameNum game # */
void showControls(int fd, int gameNum)
{
    clearScreen(fd);

    switch (gameNum)
    {

    case 0: // snake
        drawWord("controls:", 1, 1);
        drawWord("up:", 1, 10);
        drawWord("ubtn", 15, 10);

        drawWord("down:", 1, 20);
        drawWord("dbtn", 25, 20);

        drawWord("left:", OLED_WIDTH - (OLED_WIDTH / 2) + 15, 10);
        drawWord("lbtn", OLED_WIDTH - (OLED_WIDTH / 2) + 15 + 25, 10);

        drawWord("right:", OLED_WIDTH - (OLED_WIDTH / 2) + 15, 20);
        drawWord("rbtn", OLED_WIDTH - (OLED_WIDTH / 2) + 15 + 30, 20);

        sendBuffer(fd, buf);
        break;

    case 1: // pong
        drawWord("controls:", 1, 1);
        drawWord("up:", 1, 10);
        drawWord("ubtn", 15, 10);

        drawWord("down:", 1, 20);
        drawWord("dbtn", 25, 20);

        sendBuffer(fd, buf);

        break;

    case 2: // breakout
        drawWord("controls:", 1, 1);
        drawWord("left:", 1, 10);
        drawWord("lbtn", 26, 10);

        drawWord("right:", 1, 20);
        drawWord("rbtn", 31, 20);

        sendBuffer(fd, buf);
        break;

    case 3: // tetris
        drawWord("controls:", 1, 1);
        drawWord("left:", 1, 10);
        drawWord("dbtn", 26, 10);

        drawWord("right:", 50, 10);
        drawWord("ubtn", 81, 10);

        drawWord("rotate:", 1, 20);
        drawWord("mbtn", 40, 20);

        sendBuffer(fd, buf);
        sleep(3);

        clearScreen(fd);

        drawWord("please turn zedboard", 1, 1);
        drawWord("90 degrees clockwise", 1, 10);

        sendBuffer(fd, buf);
        break;
    }

    sleep(3);
}

/* @brief shows controls for each game
   @param fd oled file descriptor
   @param gameNum game #
   @param win win or lose if applicable
   @param score final score achieved in game if applicable*/
void gameOver(int fd, int gameNum, int win, int score)
{

    clearScreen(fd);

    int digits = snprintf(NULL, 0, "%d", score); // Determine the number of digits in the integer
    char str[digits + 1];                        // Allocate just enough space for the digits and null terminator

    snprintf(str, sizeof(str), "%d", score);

    switch (gameNum)
    {

    case 0: // snake
        drawWord("game over", 1, 1);
        drawWord("score:", 1, 10);
        drawWord(str, 32, 10);

        sendBuffer(fd, buf);
        break;

    case 1: // pong
        drawWord("game over", 1, 1);

        if (win == 1)
        {
            drawWord("you win", 1, 10);
        }

        else
        {
            drawWord("you lose", 1, 10);
        }

        sendBuffer(fd, buf);

        break;

    case 2: // breakout
        drawWord("game over", 1, 1);
        drawWord("score:", 1, 10);
        drawWord(str, 32, 10);

        sendBuffer(fd, buf);
        break;

    case 3: // tetris
        drawWord("game over", 1, 1);
        drawWord("score:", 1, 10);
        drawWord(str, 32, 10);

        sendBuffer(fd, buf);
        break;
    }

    sleep(3);
}

int clearScreen(int fd)
{
    int i;

    for (i = 0; (OLED_HEIGHT * OLED_WIDTH / 8) > i; i++)
    {
        buf[i] = 0;
    }
    return 0;
}

/* @brief Sends buf to OLED char device
   @param fd file to write to
   @param buf byte buffer pointer
   @return 0 on success, < 0 on error */
int sendBuffer(int fd, unsigned char *buf)
{
    int i;

    for (i = 0; i < ((OLED_HEIGHT * OLED_WIDTH) / 8); i++)
    {

        if (write(fd, buf, (OLED_HEIGHT * OLED_WIDTH) / 8) < 0)
        {
            printf("Write to Char device failed\n");
            return -1;
        }
    }

    return 0;
}

/* @brief Opens OLED char device
   @return file descriptor on success, < 0 on error */
int oledOpen()
{
    // int fd;

    fd = open("/dev/zedoled1", O_WRONLY);

    if (fd < 0)
    {
        printf("Failed to open the Char device");
        return -1;
    }

    return fd;
}

/* @brief Closes OLED char device
    @param fd file descirptor for zedoled device
   @return 0 on success, < 0 on error */
int oledClose(int fd)
{
    int err;

    err = close(fd);

    if (err == -1)
    {
        printf("Close operation failed - enable");
        return -1;
    }
    return 0;
}