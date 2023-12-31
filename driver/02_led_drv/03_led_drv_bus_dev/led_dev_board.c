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

#include "led_resources.h"


static void led_dev_release(struct device *dev)
{
}

static struct resource resources[] = {
	{
		.start	= GROUP_PIN(5,3),
		.flags	= IORESOURCE_IRQ, /* 不能使用IORESOURCE_IO，否则会报错 platform 100ask_led.0: failed to claim resource 0 */
		.name = "100ask_led_pin",
	}, 
}; 


static struct platform_device board_a_led_dev = {
	.name		= "100ask_led",
	.num_resources	= ARRAY_SIZE(resources),
	.resource		= resources,

	.dev = {
		.release = led_dev_release,
	},
};

static int __init led_dev_init(void)
{
    int err;
    err = platform_device_register(&board_a_led_dev);
    return err;
}

static void __exit led_dev_exit(void)
{
    platform_device_unregister(&board_a_led_dev);
}

module_init(led_dev_init);
module_exit(led_dev_exit);

MODULE_LICENSE("GPL");

