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
#include <linux/of.h>

#include "led_opr.h"
#include "led_drv2app.h"
#include "led_resources.h"


static int g_led_pin[100];
static int g_led_cnt = 0;


static int board_demo_led_init(int which)
{
    printk("init gpio: group: %d, pin: %d\n", GROUP(g_led_pin[which]), PIN(g_led_pin[which]));

    switch(GROUP(g_led_pin[which])){
        case 0:{

        } break;

    }
    return 0;
}

static int board_demo_led_ctl(int which, char status)
{
    printk("set led: %s gpio: group: %d, pin: %d\n", status? "on" : "off",GROUP(g_led_pin[which]), PIN(g_led_pin[which]));
    
    switch(GROUP(g_led_pin[which])){
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
    int err = 0, led_pin = 0;
    struct device_node *np;
    // 记录gpio引脚
    
    np = pdev->dev.of_node;
    if(!np){
        return -1;
    }
    // 从设备树里根据pin属性，取出led引脚
    err = of_property_read_u32(np, "pin", &led_pin);
    
    g_led_pin[g_led_cnt] = led_pin;

    // device_create
    led_device_create(g_led_cnt);
    g_led_cnt++;
    
    printk("led_drv_probe, led_pin:%d\n", led_pin);

    return 0;
}


static int led_drv_remove(struct platform_device *pdev)
{
    struct device_node *np;
    int i = 0, err = 0, led_pin = 0;
   
    // 从设备树里根据pin属性，取出led引脚
    np = pdev->dev.of_node;
    if(!np){
        return -1;
    }
    err = of_property_read_u32(np, "pin", &led_pin);

    // 销毁文件系统设备节点
    for (i=0; i<g_led_cnt; i++){
        if(g_led_pin[i] == led_pin){
            led_device_destory(i);
            g_led_pin[i] = -1;
            break;
        }
    }

    for(i=0; i<g_led_cnt; i++){
        if(g_led_pin[i] != -1){
            break;
        }
    }

    if(i == g_led_cnt){
        g_led_cnt = 0;
    }
	return 0;
}

static const struct of_device_id imx6ull_led_of_match[] = {
	{ .compatible = "imx6ull,led"},
	{},
};

static struct platform_driver board_a_led_drv = {
	.probe		= led_drv_probe,
	.remove		= led_drv_remove,
	.driver		= {
		.name	= "100ask_led",
        .of_match_table = imx6ull_led_of_match,
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

module_exit(led_drv_exit);
module_init(led_drv_init);

MODULE_LICENSE("GPL");