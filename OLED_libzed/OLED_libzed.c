#include <stdio.h>
#include <errno.h>

#include <libzed/axi_gpio.h>
#include <libzed/zed_common.h>
#include <libzed/zed_oled.h>

#define OLED_WIDTH 128
#define OLED_HEIGHT 32

void drawLetter(struct zedoled_data *inst, char letter, int origin_x, int origin_y)
{
    // every letter is 4 wide and 5 tall
    switch (letter)
    {
    case 'a':
        zedoled_set_pixel(inst, 1 + origin_x, 0 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 0 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 1 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 1 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 2 + origin_y, 1);
        zedoled_set_pixel(inst, 1 + origin_x, 2 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 2 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 2 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 3 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 3 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 4 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 4 + origin_y, 1);
        break;

    case 'b':
        zedoled_set_pixel(inst, 0 + origin_x, 0 + origin_y, 1);
        zedoled_set_pixel(inst, 1 + origin_x, 0 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 0 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 1 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 1 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 2 + origin_y, 1);
        zedoled_set_pixel(inst, 1 + origin_x, 2 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 2 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 3 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 3 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 4 + origin_y, 1);
        zedoled_set_pixel(inst, 1 + origin_x, 4 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 4 + origin_y, 1);

        break;

    case 'c':
        zedoled_set_pixel(inst, 1 + origin_x, 0 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 0 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 0 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 1 + origin_y, 1);
        zedoled_set_pixel(inst, 0 + origin_x, 2 + origin_y, 1);
        zedoled_set_pixel(inst, 0 + origin_x, 3 + origin_y, 1);

        zedoled_set_pixel(inst, 1 + origin_x, 4 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 4 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 4 + origin_y, 1);

        break;

    case 'd':

        zedoled_set_pixel(inst, 0 + origin_x, 0 + origin_y, 1);
        zedoled_set_pixel(inst, 1 + origin_x, 0 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 0 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 1 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 1 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 2 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 2 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 3 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 3 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 4 + origin_y, 1);
        zedoled_set_pixel(inst, 1 + origin_x, 4 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 4 + origin_y, 1);

        break;

    case 'e':
        zedoled_set_pixel(inst, 0 + origin_x, 0 + origin_y, 1);
        zedoled_set_pixel(inst, 1 + origin_x, 0 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 0 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 0 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 1 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 2 + origin_y, 1);
        zedoled_set_pixel(inst, 1 + origin_x, 2 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 2 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 3 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 4 + origin_y, 1);
        zedoled_set_pixel(inst, 1 + origin_x, 4 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 4 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 4 + origin_y, 1);

        break;

    case 'f':
        zedoled_set_pixel(inst, 0 + origin_x, 0 + origin_y, 1);
        zedoled_set_pixel(inst, 1 + origin_x, 0 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 0 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 0 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 1 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 2 + origin_y, 1);
        zedoled_set_pixel(inst, 1 + origin_x, 2 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 2 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 3 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 4 + origin_y, 1);

        break;

    case 'g':
        zedoled_set_pixel(inst, 1 + origin_x, 0 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 0 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 0 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 1 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 2 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 2 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 2 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 3 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 3 + origin_y, 1);

        zedoled_set_pixel(inst, 1 + origin_x, 4 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 4 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 4 + origin_y, 1);

        break;

    case 'h':
        zedoled_set_pixel(inst, 0 + origin_x, 0 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 0 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 1 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 1 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 2 + origin_y, 1);
        zedoled_set_pixel(inst, 1 + origin_x, 2 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 2 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 2 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 3 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 3 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 4 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 4 + origin_y, 1);

        break;

    case 'i':
        zedoled_set_pixel(inst, 1 + origin_x, 0 + origin_y, 1);
        zedoled_set_pixel(inst, 1 + origin_x, 1 + origin_y, 1);
        zedoled_set_pixel(inst, 1 + origin_x, 2 + origin_y, 1);
        zedoled_set_pixel(inst, 1 + origin_x, 3 + origin_y, 1);
        zedoled_set_pixel(inst, 1 + origin_x, 4 + origin_y, 1);

        break;

    case 'j':
        zedoled_set_pixel(inst, 3 + origin_x, 0 + origin_y, 1);

        zedoled_set_pixel(inst, 3 + origin_x, 1 + origin_y, 1);

        zedoled_set_pixel(inst, 3 + origin_x, 2 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 3 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 3 + origin_y, 1);

        zedoled_set_pixel(inst, 1 + origin_x, 4 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 4 + origin_y, 1);

        break;

    case 'k':
        zedoled_set_pixel(inst, 0 + origin_x, 0 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 0 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 1 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 1 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 2 + origin_y, 1);
        zedoled_set_pixel(inst, 1 + origin_x, 2 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 3 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 3 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 4 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 4 + origin_y, 1);

        break;

    case 'l':
        zedoled_set_pixel(inst, 0 + origin_x, 0 + origin_y, 1);
        zedoled_set_pixel(inst, 0 + origin_x, 1 + origin_y, 1);
        zedoled_set_pixel(inst, 0 + origin_x, 2 + origin_y, 1);
        zedoled_set_pixel(inst, 0 + origin_x, 3 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 4 + origin_y, 1);
        zedoled_set_pixel(inst, 1 + origin_x, 4 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 4 + origin_y, 1);
        break;

    case 'm':
        zedoled_set_pixel(inst, 0 + origin_x, 0 + origin_y, 1);
        zedoled_set_pixel(inst, 4 + origin_x, 0 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 1 + origin_y, 1);
        zedoled_set_pixel(inst, 1 + origin_x, 1 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 1 + origin_y, 1);
        zedoled_set_pixel(inst, 4 + origin_x, 1 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 2 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 2 + origin_y, 1);
        zedoled_set_pixel(inst, 4 + origin_x, 2 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 3 + origin_y, 1);
        zedoled_set_pixel(inst, 4 + origin_x, 3 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 4 + origin_y, 1);
        zedoled_set_pixel(inst, 4 + origin_x, 4 + origin_y, 1);
        break;

    case 'n':
        zedoled_set_pixel(inst, 0 + origin_x, 0 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 0 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 1 + origin_y, 1);
        zedoled_set_pixel(inst, 1 + origin_x, 1 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 1 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 2 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 2 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 2 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 3 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 3 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 4 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 4 + origin_y, 1);
        break;

    case 'o':
        zedoled_set_pixel(inst, 1 + origin_x, 0 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 0 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 1 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 1 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 2 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 2 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 3 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 3 + origin_y, 1);

        zedoled_set_pixel(inst, 1 + origin_x, 4 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 4 + origin_y, 1);

        break;

    case 'p':

        zedoled_set_pixel(inst, 0 + origin_x, 0 + origin_y, 1);
        zedoled_set_pixel(inst, 1 + origin_x, 0 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 0 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 1 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 1 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 2 + origin_y, 1);
        zedoled_set_pixel(inst, 1 + origin_x, 2 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 2 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 3 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 4 + origin_y, 1);

        break;

    case 'q':
        zedoled_set_pixel(inst, 1 + origin_x, 0 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 0 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 1 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 1 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 2 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 2 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 3 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 3 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 3 + origin_y, 1);

        zedoled_set_pixel(inst, 1 + origin_x, 4 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 4 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 4 + origin_y, 1);

        break;

    case 'r':
        zedoled_set_pixel(inst, 0 + origin_x, 0 + origin_y, 1);
        zedoled_set_pixel(inst, 1 + origin_x, 0 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 0 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 1 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 1 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 2 + origin_y, 1);
        zedoled_set_pixel(inst, 1 + origin_x, 2 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 2 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 3 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 3 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 4 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 4 + origin_y, 1);

        break;

    case 's':
        zedoled_set_pixel(inst, 1 + origin_x, 0 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 0 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 0 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 1 + origin_y, 1);

        zedoled_set_pixel(inst, 1 + origin_x, 2 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 2 + origin_y, 1);

        zedoled_set_pixel(inst, 3 + origin_x, 3 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 4 + origin_y, 1);
        zedoled_set_pixel(inst, 1 + origin_x, 4 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 4 + origin_y, 1);

        break;

    case 't':
        zedoled_set_pixel(inst, 0 + origin_x, 0 + origin_y, 1);
        zedoled_set_pixel(inst, 1 + origin_x, 0 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 0 + origin_y, 1);

        zedoled_set_pixel(inst, 1 + origin_x, 1 + origin_y, 1);

        zedoled_set_pixel(inst, 1 + origin_x, 2 + origin_y, 1);

        zedoled_set_pixel(inst, 1 + origin_x, 3 + origin_y, 1);

        zedoled_set_pixel(inst, 1 + origin_x, 4 + origin_y, 1);

        break;

    case 'u':
        zedoled_set_pixel(inst, 0 + origin_x, 0 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 0 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 1 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 1 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 2 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 2 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 3 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 3 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 4 + origin_y, 1);
        zedoled_set_pixel(inst, 1 + origin_x, 4 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 4 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 4 + origin_y, 1);

        break;

    case 'v':
        zedoled_set_pixel(inst, 0 + origin_x, 0 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 0 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 1 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 1 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 2 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 2 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 3 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 3 + origin_y, 1);

        zedoled_set_pixel(inst, 1 + origin_x, 4 + origin_y, 1);

        break;

    case 'w':
        zedoled_set_pixel(inst, 0 + origin_x, 0 + origin_y, 1);
        zedoled_set_pixel(inst, 4 + origin_x, 0 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 1 + origin_y, 1);
        zedoled_set_pixel(inst, 4 + origin_x, 1 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 2 + origin_y, 1);
        zedoled_set_pixel(inst, 4 + origin_x, 2 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 3 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 3 + origin_y, 1);
        zedoled_set_pixel(inst, 4 + origin_x, 3 + origin_y, 1);

        zedoled_set_pixel(inst, 1 + origin_x, 4 + origin_y, 1);

        break;

    case 'x':
        zedoled_set_pixel(inst, 0 + origin_x, 0 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 0 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 1 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 1 + origin_y, 1);

        zedoled_set_pixel(inst, 1 + origin_x, 2 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 3 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 3 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 4 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 4 + origin_y, 1);

        break;

    case 'y':
        zedoled_set_pixel(inst, 0 + origin_x, 0 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 0 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 1 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 1 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 2 + origin_y, 1);
        zedoled_set_pixel(inst, 1 + origin_x, 2 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 2 + origin_y, 1);

        zedoled_set_pixel(inst, 1 + origin_x, 3 + origin_y, 1);

        zedoled_set_pixel(inst, 1 + origin_x, 4 + origin_y, 1);

        break;

    case 'z':
        zedoled_set_pixel(inst, 0 + origin_x, 0 + origin_y, 1);
        zedoled_set_pixel(inst, 1 + origin_x, 0 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 0 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 0 + origin_y, 1);

        zedoled_set_pixel(inst, 3 + origin_x, 1 + origin_y, 1);

        zedoled_set_pixel(inst, 2 + origin_x, 2 + origin_y, 1);

        zedoled_set_pixel(inst, 1 + origin_x, 3 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 4 + origin_y, 1);
        zedoled_set_pixel(inst, 1 + origin_x, 4 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 4 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 4 + origin_y, 1);
        break;

    case ':':
        zedoled_set_pixel(inst, 0 + origin_x, 0 + origin_y, 1);
        zedoled_set_pixel(inst, 1 + origin_x, 0 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 1 + origin_y, 1);
        zedoled_set_pixel(inst, 1 + origin_x, 1 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 3 + origin_y, 1);
        zedoled_set_pixel(inst, 1 + origin_x, 3 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 4 + origin_y, 1);
        zedoled_set_pixel(inst, 1 + origin_x, 4 + origin_y, 1);

        break;

    case ' ':
        break;

    default: 
        zedoled_set_pixel(inst, 0 + origin_x, 0 + origin_y, 1);
        zedoled_set_pixel(inst, 1 + origin_x, 0 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 0 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 0 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 1 + origin_y, 1);
        zedoled_set_pixel(inst, 1 + origin_x, 1 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 1 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 1 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 2 + origin_y, 1);
        zedoled_set_pixel(inst, 1 + origin_x, 2 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 2 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 2 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 3 + origin_y, 1);
        zedoled_set_pixel(inst, 1 + origin_x, 3 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 3 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 3 + origin_y, 1);

        zedoled_set_pixel(inst, 0 + origin_x, 4 + origin_y, 1);
        zedoled_set_pixel(inst, 1 + origin_x, 4 + origin_y, 1);
        zedoled_set_pixel(inst, 2 + origin_x, 4 + origin_y, 1);
        zedoled_set_pixel(inst, 3 + origin_x, 4 + origin_y, 1);
    }
}

void drawWord(struct zedoled_data *inst, char *str, int row)
{
    int i = 0;
    int origin_y, origin_x = 1;

    origin_y = origin_y + 4 * row;

    printf("%d/n", origin_y);

    while (str[i] != '\0')
    {
        drawLetter(inst, str[i], origin_x, origin_y);
        i++;
        origin_x += 5;
    }
}

int main()
{
    struct zedoled_data *inst;
    int ret;

    inst = zedoled_get();

    ret = zedoled_initialize(inst);

    if (ret != 0)
    {
        printf("OLED init failed");
        return EINVAL;
    }

    drawWord(inst, "welcome to snake game:", 1);
    drawWord(inst, "middle button to start", 3);

    zedoled_update(inst);

    sleep(1);

    return 0;
}
