#include <stdio.h>

#include <libzed/axi_gpio.h>
#include <libzed/zed_common.h>
#include <libzed/zed_oled.h>

void drawLetter(struct zedoled_data *inst, char letter, int offset_x, int offset_y)
{
    // every letter is 4 wide and 5 tall
    switch (letter)
    {
    case 'a':
        zedoled_set_pixel(inst, 1 + offset_x, 0 + offset_y, 1);
        zedoled_set_pixel(inst, 2 + offset_x, 0 + offset_y, 1);

        zedoled_set_pixel(inst, 0 + offset_x, 1 + offset_y, 1);
        zedoled_set_pixel(inst, 3 + offset_x, 1 + offset_y, 1);

        zedoled_set_pixel(inst, 0 + offset_x, 2 + offset_y, 1);
        zedoled_set_pixel(inst, 1 + offset_x, 2 + offset_y, 1);
        zedoled_set_pixel(inst, 2 + offset_x, 2 + offset_y, 1);
        zedoled_set_pixel(inst, 3 + offset_x, 2 + offset_y, 1);

        zedoled_set_pixel(inst, 0 + offset_x, 3 + offset_y, 1);
        zedoled_set_pixel(inst, 3 + offset_x, 3 + offset_y, 1);

        zedoled_set_pixel(inst, 0 + offset_x, 4 + offset_y, 1);
        zedoled_set_pixel(inst, 3 + offset_x, 4 + offset_y, 1);
        break;

    case 'b':
        zedoled_set_pixel(inst, 0 + offset_x, 0 + offset_y, 1);
        zedoled_set_pixel(inst, 1 + offset_x, 0 + offset_y, 1);
        zedoled_set_pixel(inst, 2 + offset_x, 0 + offset_y, 1);

        zedoled_set_pixel(inst, 0 + offset_x, 1 + offset_y, 1);
        zedoled_set_pixel(inst, 3 + offset_x, 1 + offset_y, 1);

        zedoled_set_pixel(inst, 0 + offset_x, 2 + offset_y, 1);
        zedoled_set_pixel(inst, 1 + offset_x, 2 + offset_y, 1);
        zedoled_set_pixel(inst, 2 + offset_x, 2 + offset_y, 1);

        zedoled_set_pixel(inst, 0 + offset_x, 3 + offset_y, 1);
        zedoled_set_pixel(inst, 3 + offset_x, 3 + offset_y, 1);

        zedoled_set_pixel(inst, 0 + offset_x, 4 + offset_y, 1);
        zedoled_set_pixel(inst, 1 + offset_x, 4 + offset_y, 1);
        zedoled_set_pixel(inst, 2 + offset_x, 4 + offset_y, 1);

        break;

    case 'c':
        zedoled_set_pixel(inst, 1 + offset_x, 0 + offset_y, 1);
        zedoled_set_pixel(inst, 2 + offset_x, 0 + offset_y, 1);
        zedoled_set_pixel(inst, 3 + offset_x, 0 + offset_y, 1);

        zedoled_set_pixel(inst, 0 + offset_x, 1 + offset_y, 1);
        zedoled_set_pixel(inst, 0 + offset_x, 2 + offset_y, 1);
        zedoled_set_pixel(inst, 0 + offset_x, 3 + offset_y, 1);

        zedoled_set_pixel(inst, 1 + offset_x, 4 + offset_y, 1);
        zedoled_set_pixel(inst, 2 + offset_x, 4 + offset_y, 1);
        zedoled_set_pixel(inst, 3 + offset_x, 4 + offset_y, 1);

        break;

    case 'd':

        zedoled_set_pixel(inst, 0 + offset_x, 0 + offset_y, 1);
        zedoled_set_pixel(inst, 1 + offset_x, 0 + offset_y, 1);
        zedoled_set_pixel(inst, 2 + offset_x, 0 + offset_y, 1);

        zedoled_set_pixel(inst, 0 + offset_x, 1 + offset_y, 1);
        zedoled_set_pixel(inst, 3 + offset_x, 1 + offset_y, 1);

        zedoled_set_pixel(inst, 0 + offset_x, 2 + offset_y, 1);
        zedoled_set_pixel(inst, 3 + offset_x, 2 + offset_y, 1);

        zedoled_set_pixel(inst, 0 + offset_x, 3 + offset_y, 1);
        zedoled_set_pixel(inst, 3 + offset_x, 3 + offset_y, 1);

        zedoled_set_pixel(inst, 0 + offset_x, 4 + offset_y, 1);
        zedoled_set_pixel(inst, 1 + offset_x, 4 + offset_y, 1);
        zedoled_set_pixel(inst, 2 + offset_x, 4 + offset_y, 1);

        break;

    case 'e':
        zedoled_set_pixel(inst, 0 + offset_x, 0 + offset_y, 1);
        zedoled_set_pixel(inst, 1 + offset_x, 0 + offset_y, 1);
        zedoled_set_pixel(inst, 2 + offset_x, 0 + offset_y, 1);
        zedoled_set_pixel(inst, 3 + offset_x, 0 + offset_y, 1);

        zedoled_set_pixel(inst, 0 + offset_x, 1 + offset_y, 1);

        zedoled_set_pixel(inst, 0 + offset_x, 2 + offset_y, 1);
        zedoled_set_pixel(inst, 1 + offset_x, 2 + offset_y, 1);
        zedoled_set_pixel(inst, 2 + offset_x, 2 + offset_y, 1);

        zedoled_set_pixel(inst, 0 + offset_x, 3 + offset_y, 1);

        zedoled_set_pixel(inst, 0 + offset_x, 4 + offset_y, 1);
        zedoled_set_pixel(inst, 1 + offset_x, 4 + offset_y, 1);
        zedoled_set_pixel(inst, 2 + offset_x, 4 + offset_y, 1);
        zedoled_set_pixel(inst, 3 + offset_x, 4 + offset_y, 1);

        break;

    case 'f':
        break;

    case 'g':
        break;
    case 'h':
        break;
    case 'i':
        break;
    case 'j':
        break;
    case 'k':
        break;
    case 'l':
        zedoled_set_pixel(inst, 1 + offset_x, 0 + offset_y, 1);
        zedoled_set_pixel(inst, 1 + offset_x, 1 + offset_y, 1);
        zedoled_set_pixel(inst, 1 + offset_x, 2 + offset_y, 1);
        zedoled_set_pixel(inst, 1 + offset_x, 3 + offset_y, 1);
        zedoled_set_pixel(inst, 1 + offset_x, 4 + offset_y, 1);
        break;

    case 'm':
        zedoled_set_pixel(inst, 0 + offset_x, 0 + offset_y, 1);
        zedoled_set_pixel(inst, 4 + offset_x, 0 + offset_y, 1);

        zedoled_set_pixel(inst, 0 + offset_x, 1 + offset_y, 1);
        zedoled_set_pixel(inst, 1 + offset_x, 1 + offset_y, 1);
        zedoled_set_pixel(inst, 3 + offset_x, 1 + offset_y, 1);
        zedoled_set_pixel(inst, 4 + offset_x, 1 + offset_y, 1);

        zedoled_set_pixel(inst, 0 + offset_x, 2 + offset_y, 1);
        zedoled_set_pixel(inst, 2 + offset_x, 2 + offset_y, 1);
        zedoled_set_pixel(inst, 4 + offset_x, 2 + offset_y, 1);

        zedoled_set_pixel(inst, 0 + offset_x, 3 + offset_y, 1);
        zedoled_set_pixel(inst, 4 + offset_x, 3 + offset_y, 1);

        zedoled_set_pixel(inst, 0 + offset_x, 4 + offset_y, 1);
        zedoled_set_pixel(inst, 4 + offset_x, 4 + offset_y, 1);
        break;

    case 'n':
        zedoled_set_pixel(inst, 0 + offset_x, 0 + offset_y, 1);
        zedoled_set_pixel(inst, 3 + offset_x, 0 + offset_y, 1);

        zedoled_set_pixel(inst, 0 + offset_x, 1 + offset_y, 1);
        zedoled_set_pixel(inst, 1 + offset_x, 1 + offset_y, 1);
        zedoled_set_pixel(inst, 3 + offset_x, 1 + offset_y, 1);

        zedoled_set_pixel(inst, 0 + offset_x, 2 + offset_y, 1);
        zedoled_set_pixel(inst, 2 + offset_x, 2 + offset_y, 1);
        zedoled_set_pixel(inst, 3 + offset_x, 2 + offset_y, 1);

        zedoled_set_pixel(inst, 0 + offset_x, 3 + offset_y, 1);
        zedoled_set_pixel(inst, 3 + offset_x, 3 + offset_y, 1);

        zedoled_set_pixel(inst, 0 + offset_x, 4 + offset_y, 1);
        zedoled_set_pixel(inst, 3 + offset_x, 4 + offset_y, 1);
        break;

    case 'o':
        break;
    case 'p':
        break;
    case 'q':
        break;
    case 'r':
        break;
    case 's':
        break;
    case 't':
        break;
    case 'u':
        break;
    case 'v':
        break;
    case 'w':
        break;
    case 'x':
        break;
    case 'y':
        break;
    case 'z':
        break;
    case ':':
        break;
    case default:
    }
}

void drawWord(struct zedoled_data *inst, char *str)
{
    int i = 0;
    int offset_y, offset_x = 1, 1;

    while (str[i] != '\0')
    {
        drawLetter(inst, str[i], offset_x, offset_y);
        i++;
        offset_x += 5;
        offset_y += 5;
    }
}

int main()
{
    struct zedoled_data *inst;
    int ret;

    inst = zedoled_get(void);

    ret = zedoled_initialize(struct zedoled_data * inst);

    if (ret != 0)
    {
        printf("OLED init failed");
        return EINVAL;
    }

    zedoled_all_on(inst);

    sleep(1);

    zedoled_all_off(inst);

    drawLetter(inst, 'b', 0, 0);
    drawLetter(inst, 'a', 5, 0);
    drawLetter(inst, 'l', 10, 0);
    drawLetter(inst, 'l', 15, 0);

    zedoled_update(inst);

    sleep(1);

    zedoled_all_off(inst);

    drawWord(inst, "menu");

    sleep(1);

    return 0;
}