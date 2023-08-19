#include <linux/module.h>

#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/stat.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/tty.h>
#include <linux/kmod.h>
#include <linux/gfp.h>
#include <linux/platform_device.h>

#include "led_opr.h"
#include "led_drv2app.h"
#include "led_resources.h"


static int g_leds[100];
static int g_led_cnt = 0;


static int board_demo_led_init(int which)
{
    printk("init gpio: group: %d, pin: %d\n", GROUP(g_leds[which]), PIN(g_leds[which]));

    switch(GROUP(g_leds[which])){
        case 0:{

        } break;

    }
    return 0;
}

static int board_demo_led_ctl(int which, char status)
{
    printk("set led: %s gpio: group: %d, pin: %d\n", status? "on" : "off",GROUP(g_leds[which]), PIN(g_leds[which]));
    
    switch(GROUP(g_leds[which])){
        case 0:{

        } break;

    }
    return 0;
}

static struct led_operations  board_demo_led_ops = {
    .init = board_demo_led_init,
    .ctl  = board_demo_led_ctl,
};

struct led_operations *get_board_led_opr(void)
{
    return &board_demo_led_ops;
}


static int led_drv_probe(struct platform_device *pdev)
{
    int i = 0;
    struct resource *res;
    // 记录gpio引脚
    for(;;){
        res = platform_get_resource(pdev, IORESOURCE_IRQ, i++);
        if(!res)
            break;
        g_leds[g_led_cnt] = res->start;

        // device_create
        led_device_create(g_led_cnt);
        g_led_cnt++;
    }
    
    return 0;
}


static int led_drv_remove(struct platform_device *pdev)
{
    struct resource *res;
    int i = 0;

    // device_destory
    while (1){
        res = platform_get_resource(pdev, IORESOURCE_IRQ, i);
        if (!res)
            break;
        
        led_device_destory(i);
        i++;
        g_led_cnt--;
    }

	return 0;
}

static struct platform_driver board_a_led_drv = {
	.probe		= led_drv_probe,
	.remove		= led_drv_remove,
	.driver		= {
		.name	= "100ask_led",
	},
};

static int __init led_drv_init(void)
{
    int err;
    err = platform_driver_register(&board_a_led_drv);
    register_led_operations(&board_demo_led_ops);
    return 0;
}

static void __exit led_drv_exit(void)
{
    platform_driver_unregister(&board_a_led_drv);
}

module_init(led_drv_init);
module_exit(led_drv_exit);

MODULE_LICENSE("GPL");