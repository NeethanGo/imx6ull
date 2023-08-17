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

#include "led_opr.h"


static volatile unsigned int *CCM_CCGR1;
static volatile unsigned int *IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3;
static volatile unsigned int *GPIO5_GDIR;
static volatile unsigned int *GPIO5_DR;


static int board_demo_led_init(int which)
{
    printk("%s %s lind %d, led %d\n", __FILE__, __FUNCTION__, __LINE__, which);
    
    if(which == 0){
        if (!CCM_CCGR1){
            CCM_CCGR1 = ioremap(0x20c406c, 4);
            IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 = ioremap(0x2290014, 4);
            GPIO5_GDIR = ioremap(0x20ac004, 4);
            GPIO5_DR = ioremap(0x20AC000, 4);
        }

        // 使能gpio5时钟 CCM_CCGR1
        // CCGR1[CG15](GPIO5_CLK_ENABLE), 20c_406ch
        // bit[31:30] = 0b11
        *CCM_CCGR1 |= (3 << 30);

        // 设置gpio5_io03用于gpio
        // IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3, 229_0014h
        // bit[3:0] = 0b0101 alt4
        *IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 &= ~(0x0f);
        *IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 |= 5<<0;

        // gpio5_io03为输出功能
        // (GPIO5_GDIR),20A_C004
        // bit[3] = 1
        *GPIO5_GDIR |= (1<<3);
    }

    return 0;
}

static int board_demo_led_ctl(int which, char status)
{
    printk("%s %s lind %d, led %d, %s\n", __FILE__, __FUNCTION__, __LINE__, which, status? "on":"off");
    
     if(which == 0){
        if(status == 1){ // 点灯
            // 设置gpio5_3输出高电平
            // (GPIO5_DR)，20A_C000
            // bit[1] = 0
            *GPIO5_DR &= ~(1<<3); 
        } else {
            // 关灯
             // bit[1] = 1
             *GPIO5_DR |= (1<<3); 
        }
     
     }
    return 0;
}

static struct led_operations  board_demo_led_ops = {
    .num  = 1,
    .init = board_demo_led_init,
    .ctl  = board_demo_led_ctl,
};

struct led_operations *get_board_led_opr(void)
{
    return &board_demo_led_ops;
}