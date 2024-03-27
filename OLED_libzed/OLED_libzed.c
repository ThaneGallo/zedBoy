#include <libzed/axi_gpio.h>
#include <libzed/zed_common.h>
#include <libzed/zed_oled.h>

void drawLetter(struct zedoled_data *inst, char letter, int offset_x, int offset_y)
{
    switch (letter)
    {
    case a:
        zedoled_set_pixel(struct zedoled_data * inst, 1 + offset_x, 0 + offset_y, 1);
        zedoled_set_pixel(struct zedoled_data * inst, 2 + offset_x, 0 + offset_y, 1);

        zedoled_set_pixel(struct zedoled_data * inst, 0 + offset_x, 1 + offset_y, 1);
        zedoled_set_pixel(struct zedoled_data * inst, 3 + offset_x, 1 + offset_y, 1);

        zedoled_set_pixel(struct zedoled_data * inst, 0 + offset_x, 2 + offset_y, 1);
        zedoled_set_pixel(struct zedoled_data * inst, 1 + offset_x, 2 + offset_y, 1);
        zedoled_set_pixel(struct zedoled_data * inst, 2 + offset_x, 2 + offset_y, 1);
        zedoled_set_pixel(struct zedoled_data * inst, 3 + offset_x, 2 + offset_y, 1);

        zedoled_set_pixel(struct zedoled_data * inst, 0 + offset_x, 3 + offset_y, 1);
        zedoled_set_pixel(struct zedoled_data * inst, 3 + offset_x, 3 + offset_y, 1);

        zedoled_set_pixel(struct zedoled_data * inst, 0 + offset_x, 4 + offset_y, 1);
        zedoled_set_pixel(struct zedoled_data * inst, 3 + offset_x, 4 + offset_y, 1);

    case b:

    case c:

    case d:

    case e:

    case f:
    case g:
    case h:
    case i:
    case j:
    case k:
    case l:
    case m:
    case n:
    case o:
    case p:
    case q:
    case r:
    case s:
    case t:
    case u:
    case v:
    case w:
    case x:
    case y:
    case z:
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

    drawLetter(inst, 'a', 0, 0);
    drawLetter(inst, 'a', 5, 0);
    drawLetter(inst, 'a', 10, 0);

    return 0;
}