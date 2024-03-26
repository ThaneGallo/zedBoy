#include <libzed/axi_gpio.h>
#include <libzed/zed_common.h>
#include <libzed/zed_oled.h>




int main(){
    struct zedoled_data* inst;
    int ret;

    inst = zedoled_get(void);

    ret = zedoled_initialize(struct zedoled_data* inst);

    if(ret!=0){
        printf("OLED init failed");
        return EINVAL;
    }

    zedoled_all_on(inst);

    sleep(1);

    zedoled_all_off(inst);



    return 0;
}