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

#include "button_drv.h"


/* 1. 确定主设备号                                                                 */
static int major = 0;
static struct class *button_class;
static struct button_operations *p_button_ops;

#define MIN(a, b) (a < b ? a : b)

/* 3. 实现对应的open/read/write等函数，填入file_operations结构体                   */
static ssize_t button_drv_read (struct file *file, char __user *buf, size_t size, loff_t *offset)
{
    char level;
    int err;
	int minor = iminor(file_inode(file));

	// 根据次设备号控制button
	level = p_button_ops->read(minor);
    err = copy_to_user(buf, &level, 1);
    printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	return 1;
}

static ssize_t button_drv_write (struct file *file, const char __user *buf, size_t size, loff_t *offset)
{
	// int err;
	// char status;
	// int minor = iminor(file_inode(file));
	// printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	// err = copy_from_user(&status, buf, 1);

	// 根据次设备号和status控制button
	// p_button_ops->ctl(minor, status);

	return 1;
}

static int button_drv_open (struct inode *inode, struct file *file)
{
	int minor = iminor(inode);
	
	// 根据次设备号初始化button
	p_button_ops->init(minor);
	return 0;
}

static int button_drv_close (struct inode *node, struct file *file)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	return 0;
}

/* 2. 定义自己的file_operations结构体                                              */
static struct file_operations button_fops = {
	.owner	 = THIS_MODULE,
	.open    = button_drv_open,
	.read    = button_drv_read,
	.write   = button_drv_write,
	.release = button_drv_close,
};

void register_button_operations(struct button_operations *ops)
{
    p_button_ops = ops;
}


/* 4. 把file_operations结构体告诉内核：注册驱动程序                                */
/* 5. 谁来注册驱动程序啊？得有一个入口函数：安装驱动程序时，就会去调用这个入口函数 */
static int __init button_init(void)
{
	int err, i;
	
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	major = register_chrdev(0, "100ask_button", &button_fops);  /* /dev/button */

	button_class = class_create(THIS_MODULE, "button_class");
	err = PTR_ERR(button_class);
	if (IS_ERR(button_class)) {
		printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
		unregister_chrdev(major, "100ask_button");
		return -1;
	}

    p_button_ops = get_board_button_opr();
    for(i=0; i<p_button_ops->count; i++){
        device_create(button_class, NULL, MKDEV(major, i), NULL, "100ask_button%d", i); /* /dev/100ask_button */
    }
	
	return 0;
}

/* 6. 有入口函数就应该有出口函数：卸载驱动程序时，就会去调用这个出口函数           */
static void __exit button_exit(void)
{
    int i;
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	
    for(i=0; i<p_button_ops->count; i++){
        device_destroy(button_class, MKDEV(major, i));
    }

	class_destroy(button_class);
	unregister_chrdev(major, "100ask_button");
}


/* 7. 其他完善：提供设备信息，自动创建设备节点                                     */

module_init(button_init);
module_exit(button_exit);

MODULE_LICENSE("GPL");


