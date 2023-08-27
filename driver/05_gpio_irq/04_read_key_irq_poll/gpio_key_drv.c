#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/watchdog.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/delay.h>
#include <linux/bitops.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/init.h>
#include <linux/poll.h>

struct gpio_key {
	int gpio;
	struct gpio_desc *gpiod;
	int flag;
	int irq;
};

static struct gpio_key *gpio_keys_100ask;
static struct class *gpio_key_class;
static int major;

// 初始化队列
static DECLARE_WAIT_QUEUE_HEAD(gpio_key_wait);

// 环形缓冲区
#define BUF_MAX_LEN	128
#define NEXT_POS(x)	((x+1) % BUF_MAX_LEN)
static int g_keys[BUF_MAX_LEN];
static int r = 0, w = 0;

static int is_key_buf_empty(void)
{
	return (r == w);
}

static int is_key_buf_full(void)
{
	return (r == NEXT_POS(w));
}

static void push_key(int key)
{
	if(!is_key_buf_full()){
		g_keys[w] = key;
		w = NEXT_POS(w);
	}
}

static int pop_key(void)
{
	int key = 0;
	if(!is_key_buf_empty()){
		key = g_keys[r];
		r = NEXT_POS(r);
	}
	return key;
}

static unsigned int gpio_key_poll(struct file *file, poll_table *wait)
{
	printk("%s %s\n", __FILE__, __FUNCTION__);

	// 指定在哪个队列进行休眠
	poll_wait(file, &gpio_key_wait, wait);
	return is_key_buf_empty() ? 0 : POLLIN | POLLRDNORM; // POLLRDNORM等同于POLLIN
}


/* 3. 实现对应的open/read/write等函数，填入file_operations结构体                   */
static ssize_t gpio_key_drv_read (struct file *file, char __user *buf, size_t size, loff_t *offset)
{
    int err,key;

	wait_event_interruptible(gpio_key_wait, !is_key_buf_empty()); // 进入休眠，等待事件产生
    key = pop_key();
	err = copy_to_user(buf, &key, 4);

	return 4;
}


/* 2. 定义自己的file_operations结构体                                              */
static struct file_operations gpio_key_fops = {
	.owner	 = THIS_MODULE,
	.read    = gpio_key_drv_read,
	.poll	 = gpio_key_poll,	
};

static irqreturn_t gpio_key_isr(int irq, void *dev_id)
{
	struct gpio_key *gpio_key = dev_id;
	int val, key;

	val = gpiod_get_value(gpio_key->gpiod);
	printk("key:%d, val:%d\n", gpio_key->gpio, val);

	key = (gpio_key->gpio<<8) | val;
	push_key(key);
	wake_up_interruptible(&gpio_key_wait); // 从gpio_key_wait队列里唤醒此线程

	return IRQ_HANDLED;
}


static int gpio_key_probe(struct platform_device *pdev)
{
	int i, count,err;
	struct device_node *node = pdev->dev.of_node;
	enum of_gpio_flags flag;
	unsigned flags = GPIOF_IN;

	// 获取设备树节点获取gpio的个数
	count = of_gpio_count(node);
	if(!count){
		printk("%s %s, there isn't any gpio availabe!\n", __FILE__, __FUNCTION__);
		return -1;
	}

	gpio_keys_100ask = kzalloc(sizeof(struct gpio_key) * count, GFP_KERNEL);
	for(i=0; i<count; i++){
		// 获取这个节点的第i个引脚的flag
		gpio_keys_100ask[i].gpio = of_get_gpio_flags(node, i, &flag);
		if(gpio_keys_100ask[i].gpio < 0){
			printk("%s %s, of_get_gpio_flags failed!\n", __FILE__, __FUNCTION__);
		}

		gpio_keys_100ask[i].gpiod = gpio_to_desc(gpio_keys_100ask[i].gpio);
		gpio_keys_100ask[i].flag = flag & OF_GPIO_ACTIVE_LOW;

		if(flag & OF_GPIO_ACTIVE_LOW){
			flags |= GPIOF_ACTIVE_LOW;
		}
		err = devm_gpio_request_one(&pdev->dev, gpio_keys_100ask[i].gpio, flags, NULL);

		gpio_keys_100ask[i].irq = gpio_to_irq(gpio_keys_100ask[i].gpio);
	}

	for(i=0; i<count; i++){
		err = request_irq(gpio_keys_100ask[i].irq, gpio_key_isr, IRQF_TRIGGER_RISING, "100ask_gpio_key", &gpio_keys_100ask[i]);
	}


	// 注册fops
	major = register_chrdev(0, "100ask_gpio_key", &gpio_key_fops); 

	// 让系统创建字符设备驱动节点
	gpio_key_class = class_create(THIS_MODULE, "100ask_gpio_key_class");
	if(IS_ERR(gpio_key_class)){
		unregister_chrdev(major, "100ask_gpio_key_class");
		return PTR_ERR(gpio_key_class);
	}
	device_create(gpio_key_class, NULL, MKDEV(major, 0), NULL, "100ask_gpio_key"); // /dev/100ask_gpio_key
	
	return 0;
}

static int gpio_key_remove(struct platform_device *pdev)
{
	struct device_node *node = pdev->dev.of_node;
	int count;
	int i;

	count = of_gpio_count(node);
	for (i = 0; i < count; i++){
		free_irq(gpio_keys_100ask[i].irq, &gpio_keys_100ask[i]);
	}
	kfree(gpio_keys_100ask);
    return 0;
}

static const struct of_device_id ask100_keys[] = {
    {.compatible = "100ask,gpio_key"},
    {},
};

// 1.定义platform_driver
static struct platform_driver gpio_keys_driver = {
    .probe = gpio_key_probe,
    .remove = gpio_key_remove,
    .driver = {
        .name = "100ask_gpio_key",
        .of_match_table = ask100_keys,
    },
};




// 2.在入口函数注册paltform_driver
static int __init gpio_keys_init(void)
{
	int rc;

	rc = platform_driver_register(&gpio_keys_driver);
	return rc;
}


// 3.在出口函数卸载platform_driver
static void __exit gpio_keys_exit(void)
{
    device_destroy(gpio_key_class, MKDEV(major, 0));

	class_destroy(gpio_key_class);
	unregister_chrdev(major, "100ask_gpio_key_class");

	platform_driver_unregister(&gpio_keys_driver);
}

module_init(gpio_keys_init);
module_exit(gpio_keys_exit);


MODULE_AUTHOR("Neethan <neethan@foxmail.com>");
MODULE_DESCRIPTION("IMX6 GPIO Key driver");
MODULE_LICENSE("GPL");