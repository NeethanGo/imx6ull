
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


struct imx6ull_gpio {
    volatile unsigned int dr;
    volatile unsigned int gdir;
    volatile unsigned int psr;
    volatile unsigned int icr1;
    volatile unsigned int icr2;
    volatile unsigned int imr;
    volatile unsigned int isr;
    volatile unsigned int edge_sel;
};

volatile unsigned int *CCM_CCGR3;
volatile unsigned int *IOMUXC_SW_MUX_CTL_PAD_NAND_CE1_B;
volatile unsigned int *GPIO4_GDIR;
volatile unsigned int *GPIO4_DR;
static struct imx6ull_gpio *gpio4;


static void board_button_init_gpio(int which)
{
    // key2，GPIO4_IO14
    if(!CCM_CCGR3){

        // 1.开启gpio时钟
        // CCGR1[13:12] = 0b11
        CCM_CCGR3 = ioremap(0x20C4074, 4); // 20C_4074h
        *CCM_CCGR3 |= (3 << 12);

        // 2.设置gpio4_14为gpio功能
        //  IOMUXC_SW_MUX_CTL_PAD_NAND_CE1_B[3：0] = 0b0101 ALT5
         IOMUXC_SW_MUX_CTL_PAD_NAND_CE1_B = ioremap(0x20E01B0, 4); // 20E_01B0h
        //  *IOMUXC_SW_MUX_CTL_PAD_NAND_CE1_B &= ~(5);
         *IOMUXC_SW_MUX_CTL_PAD_NAND_CE1_B |= 5;

        // 设置gpio4_14工作模式为输入
        // GPIO4_GDIR[14] = 0 INPUT
        GPIO4_GDIR = ioremap(0x20A8004, 4); // 20A_8004
        *GPIO4_GDIR &= ~(1<<14);

        printk("%s\n", __FUNCTION__);
    }
}

static int board_button_read_gpio(int which)
{
    int value = 0;

    // 3.读取gpio值
    if(!GPIO4_DR){
        gpio4 = ioremap(0x020A8000, sizeof(struct imx6ull_gpio)); // 20A_8000
    }
    value = (gpio4->psr & (1<<14)) ? 1 : 0;

    printk("%s, key2, value: %d\n", __FUNCTION__, value);
    return value;
}

static struct button_operations imx6ull_button_ops = {
    .count = 1,
    .init = board_button_init_gpio,
    .read = board_button_read_gpio,
};

struct button_operations *get_board_button_opr(void)
{
    return &imx6ull_button_ops;
}