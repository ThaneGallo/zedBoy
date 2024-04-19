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

#define OLED_WIDTH 128
#define OLED_HEIGHT 32

char buf[(OLED_HEIGHT * OLED_WIDTH) / 8];

/* @brief draws pixel
   @param x pixel x coordinate
   @param y pixel y coordinate
   @param state 1 = on 0 = off
   @return 0 on success, < 0 on error */
int fbPixelDraw(int x, int y, int state)
{
    char byte_offset;
    int pgNum;

    // page = 1 byte
    pgNum = y / 8;

    // 4 pages per column  + pg offset
    byte_offset = x * (OLED_WIDTH / 8) + pgNum;

    if (state == 1)
    {
        // pulls high original pixel state for chosen byte
        buf[byte_offset] = buf[byte_offset] | y;
    }
    else
    {
        // pulls low original pixel state for chosen byte
        buf[byte_offset] = buf[byte_offset] & ~y;
    }

    return 0;
}

/* @brief draws line between 2 points
   @param startX start pixel x coordinate
   @param startY pixel y coordinate
   @param endX pixel x coordinate
   @param endY pixel y coordinate
   @param state 1 = on 0 = off
   @return 0 on success, < 0 on error */
int drawLine(int startX, int startY, int endX, int endY, int state)
{
    int dx, dy;

    int i, j = 0;

    dx = endX - startX;
    dy = endY - startY;

    // pixel has no length and draws itself
    if ((dy == 0) && (dx == 0))
    {
        fbPixelDraw(startX, startY, state);
    }

    // draws lines left / right
    else if (dy == 0)
    {
        // right
        if (dx > 0)
        {
            for (i = 0; i < dx; i++)
            {
                fbPixelDraw(startX + i, startY, state);
                printf("in pixel draw right\n");
            }
        }
        // left
        else if (dx < 0)
        {
            for (i = 0; i + 1 > dx; i--)
            {
                fbPixelDraw(startX + i, startY, state);
                printf("in pixel draw left \n");
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
                fbPixelDraw(startX, startY + j, state);
            }
        }
        // down
        else if (dy < 0)
        {
            for (j = 0; j + 1 > dy; j--)
            {
                fbPixelDraw(startX, startY + j, state);
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

    return 0;
}

/* @brief draws empty rectangle between 2 points
   @param startX start pixel x coordinate
   @param startY pixel y coordinate
   @param endX pixel x coordinate
   @param endY pixel y coordinate
   @param state 1 = on 0 = off
   @return 0 on success, < 0 on error */
int drawEmptyRectangle(int startX, int startY, int endX, int endY, int state)
{

    drawLine(startX, startY, startX, endY, state);
    drawLine(startX, startY, endX, startY, state);
    drawLine(endX, startY, endX, endY, state);
    drawLine(startX, endY, endX, endY, state);

    return 0;
}

/* @brief draws circle
   @param centerX x coordinate of center point
   @param centerY y coordinate of center point
   @param radius raiuds of desired circle
   @return 0 on success, < 0 on error */
int drawCircle(int centerX, int centerY, int radius)
{
    if (radius > 0 && radius < 5)
    {
        switch (radius)
        {
        case 1:
            fbPixelDraw(centerX, centerY, 1);
            break;
        case 2:
            fbPixelDraw(centerX, centerY, 1);
            fbPixelDraw(centerX + 1, centerY, 1);
            fbPixelDraw(centerX - 1, centerY, 1);
            fbPixelDraw(centerX, centerY + 1, 1);
            fbPixelDraw(centerX, centerY - 1, 1);
            break;
        case 3:
            drawEmptyRectangle(centerX - 1, centerY - 1, centerX + 1, centerY + 1, 1);
            break;
        case 4:
            fbPixelDraw(centerX, centerY, 1);
            fbPixelDraw(centerX + 1, centerY, 1);
            fbPixelDraw(centerX - 1, centerY + 1, 1);
            fbPixelDraw(centerX - 1, centerY + 2, 1);
            fbPixelDraw(centerX + 2, centerY - 1, 1);
            fbPixelDraw(centerX + 2, centerY - 2, 1);
            fbPixelDraw(centerX, centerY - 3, 1);
            fbPixelDraw(centerX + 1, centerY - 3, 1);
            break;
        }
    }
    return 0;
}

/* @brief Sends buf to OLED char device
   @param fp file pointer
   @param buf byte buffer pointer
   @return 0 on success, < 0 on error */
int sendBuffer(FILE *fp, char *buf)
{

    return 0;
}

void main()
{
    FILE *fp;

    int i, j;

    for (i = 0; i < (OLED_HEIGHT * OLED_WIDTH) / 8; i++)
    {
        buf[i] = 0;
    }

    // Open the character device
    int fd = open("/dev/zedoled1", O_WRONLY);
    if (fd < 0)
    {
        printf("%sFailed to open the character device");
        return -1;
    }
}