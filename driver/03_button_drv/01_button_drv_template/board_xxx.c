
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
#include <asm/io.h>

#include "button_drv.h"


static void board_button_init_gpio(int which)
{
    printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
}

static int board_button_read_gpio(int which)
{
    printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
    return 1;
}

static struct button_operations imx6ull_button_ops = {
    .count = 2,
    .init = board_button_init_gpio,
    .read = board_button_read_gpio,
};

struct button_operations *get_board_button_opr(void)
{
    return &imx6ull_button_ops;
}