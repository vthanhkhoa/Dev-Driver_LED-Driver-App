#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/string.h>
#include <linux/time.h>

#define DEVICE 						5
#define AUTHOR 						"vthanhkhoa"
#define DESC 						"A example character device driver"
#define DEVICE_NAME 				"led_ioctl"

#define MAGIC_NO 					100
#define GET_LED_STATE 				_IOW(MAGIC_NO, 0, char)
#define LED_ON 						_IOW(MAGIC_NO, 1, char)
#define LED_OFF 					_IOW(MAGIC_NO, 2, char)
#define LED_BLINK 					_IOW(MAGIC_NO, 3, char)

#define GPIO_ADDR_BASE 				0x44E07000
#define ADDR_SIZE 					(0x1000)
#define GPIO_SETDATAOUT_OFFSET 		0x194
#define GPIO_CLEARDATAOUT_OFFSET 	0x190
#define GPIO_OE_OFFSET 				0x134
#define GPIO_IN_REG 				0x138
#define LED							~(1 << 27)
#define DATA_OUT 					(1 << 27)

void __iomem *base_addr;
uint32_t reg_data = 0;
static dev_t dev_num;
struct class *my_class;
struct device *my_dev;
struct cdev my_cdev;
struct timer_list tm_list;

void timer_handler(struct timer_list *t_list)
{
	reg_data = readl_relaxed(base_addr + GPIO_IN_REG);
	switch ((reg_data & DATA_OUT) >> 27)
	{
	case 1:
		writel_relaxed(DATA_OUT, base_addr + GPIO_CLEARDATAOUT_OFFSET);
		break;
	case 0:
		writel_relaxed(DATA_OUT, base_addr + GPIO_SETDATAOUT_OFFSET);
		break;
	};
	mod_timer(&tm_list, jiffies + 100);
}

static int my_open(struct inode *inode, struct file *file)
{
	printk("open\n");
	return 0;
}

static int my_close(struct inode *inode, struct file *file)
{
	printk("close\n");
	return 0;
}

static ssize_t my_read(struct file *flip, char __user *user_buf, size_t len, loff_t *offs)
{
	printk("read\n");
	char *data = NULL;
	data = kmalloc(len, GFP_KERNEL);
	memset(data, 0, len);
	reg_data = readl_relaxed(base_addr + GPIO_IN_REG);
	data[0] = ((reg_data & DATA_OUT) >> 27) + 48;
	copy_to_user(user_buf, data, len);
	switch (data[0])
	{
	case '1':
		printk("LED ON");
		break;
	case '0':
		printk("LED OFF");
		break;
	default:
		printk(KERN_INFO "un-support operation");
	};
	return 0;
}

static ssize_t my_write(struct file *flip, const char __user *user_buf, size_t len, loff_t *offs)
{
	char data[10];
	printk("write\n");
	memset(data, 0, 10); // init data arr[10] with value 0
	copy_from_user(data, user_buf, len);
	switch (data[0])
	{
	case '1':
		writel_relaxed(DATA_OUT, base_addr + GPIO_SETDATAOUT_OFFSET);
		break;
	case '0':
		writel_relaxed(DATA_OUT, base_addr + GPIO_CLEARDATAOUT_OFFSET);
		break;
	default:
		printk(KERN_INFO "un-support operation");
	};
	return len;
}

static long dev_ioctl(struct file *filep, unsigned int cmd, unsigned long arg)
{
	void __user *argp = (void __user *)arg;
	char __user *value = argp;
	char *data = NULL;

	switch (cmd)
	{
	case GET_LED_STATE:
		data = kmalloc(10, GFP_KERNEL);
		memset(data, 0, 10);
		reg_data = readl_relaxed(base_addr + GPIO_IN_REG);
		data[0] = ((reg_data & DATA_OUT) >> 27) + 48;
		switch (data[0])
		{
		case '1':
			printk(KERN_INFO "State:	LED ON\n");
			break;
		case '0':
			printk(KERN_INFO "State:	LED OFF\n");
			break;
		default:
			printk(KERN_INFO "	Un-support operation\n");
		};
		break;

	case LED_ON:
		del_timer(&tm_list);
		printk(KERN_INFO "write on\n");
		writel_relaxed(DATA_OUT, base_addr + GPIO_SETDATAOUT_OFFSET);
		break;

	case LED_OFF:
		del_timer(&tm_list);
		printk(KERN_INFO "write off\n");
		writel_relaxed(DATA_OUT, base_addr + GPIO_CLEARDATAOUT_OFFSET);
		break;

	case LED_BLINK:
		printk(KERN_INFO "write blink\n");
		int k = HZ;
		printk("hellu\n%d\n", k);
		timer_setup(&tm_list, timer_handler, 0);
		tm_list.expires = jiffies + HZ;
		add_timer(&tm_list);
		break;

	default:
		return -ENOTTY;
	}
	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = my_open,
	.release = my_close,
	.read = my_read,
	.write = my_write,
	.unlocked_ioctl = dev_ioctl,

};

static int __init func_init(void)
{
	printk(KERN_INFO "Hello i am vo thanh khoa\n");

	alloc_chrdev_region(&dev_num, 0, DEVICE, DEVICE_NAME);
	my_class = class_create(THIS_MODULE, DEVICE_NAME);
	cdev_init(&my_cdev, &fops);
	my_cdev.owner = THIS_MODULE;
	cdev_add(&my_cdev, dev_num, 1);
	device_create(my_class, NULL, dev_num, NULL, DEVICE_NAME);

	base_addr = ioremap(GPIO_ADDR_BASE, ADDR_SIZE);
	reg_data = readl_relaxed(base_addr + GPIO_OE_OFFSET);
	reg_data &= LED;
	writel_relaxed(reg_data, base_addr + GPIO_OE_OFFSET);

	return 0;
}

static void __exit func_exit(void)
{
	writel_relaxed(DATA_OUT, base_addr + GPIO_CLEARDATAOUT_OFFSET);
	cdev_del(&my_cdev);
	device_destroy(my_class, dev_num);
	class_destroy(my_class);
	unregister_chrdev(dev_num, DEVICE_NAME);
	del_timer(&tm_list);
	printk(KERN_INFO "Goodbye see u later\n");
}

module_init(func_init);
module_exit(func_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(AUTHOR);
MODULE_DESCRIPTION(DESC);
MODULE_VERSION("0.01");
