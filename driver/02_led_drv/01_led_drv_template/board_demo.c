#include <linux/gfp.h>
#include "led_ops.h"


static int board_demo_led_init(int which)
{
    printk("%s %s lind %d, led %d\n", __FILE__, __FUNCTION__, __LINE__, which);
    return 0;
}

static int board_demo_led_ctl(int which, char status)
{
    printk("%s %s lind %d, led %d, %s\n", __FILE__, __FUNCTION__, __LINE__, which, status? "on":"off");
    return 0;
}

static struct led_ops  board_demo_led_ops = {
    .init = board_demo_led_init,
    .ctl  = board_demo_led_ctl,
};

struct led_ops *get_board_led_ops(void)
{
    return &board_demo_led_ops;
}