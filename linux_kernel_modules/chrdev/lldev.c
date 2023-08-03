#include <linux/jiffies.h>
#include <linux/uaccess.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include "./ll_ioctl.h"

#define NUM_ELEMENTS 5
#define BUFFER_SIZE 32

static int lldev_open(struct inode *, struct file *);
static int lldev_release(struct inode *, struct file *);
static long lldev_ioctl(struct file *, unsigned int, unsigned long);
static ssize_t lldev_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t lldev_write(struct file *, const char __user *, size_t, loff_t *);
static u_long lldev_help_sum(void);
static u_long lldev_help_del_node(void);

static const struct file_operations lldev_fops = {
    .owner      = THIS_MODULE,
    .open       = lldev_open,
    .release    = lldev_release,
    .unlocked_ioctl = lldev_ioctl,
    .read       = lldev_read,
    .write      = lldev_write
};

struct cdev lldev_cdev;
static int dev_major = 0;
static struct class *lldev_class = NULL;
static char number_buffer[BUFFER_SIZE];
static u_long last_magic;
struct my_list
{
     struct list_head list;
     unsigned long magic;
};
LIST_HEAD(head_node);

static int lldev_uevent(struct device *dev, struct kobj_uevent_env *env)
{
    add_uevent_var(env, "DEVMODE=%#o", 0666);
    return 0;
}

static int __init lldev_init(void)
{
    int err, i;
    dev_t dev;
    struct my_list *node, *temp;

	pr_info("ll_driver - Module init\n");
    err = alloc_chrdev_region(&dev, 0, 1, "lldev");
    dev_major = MAJOR(dev);

    lldev_class = class_create(THIS_MODULE, "lldev");
    lldev_class->dev_uevent = lldev_uevent;

    cdev_init(&lldev_cdev, &lldev_fops);
    lldev_cdev.owner = THIS_MODULE;
    cdev_add(&lldev_cdev, MKDEV(dev_major, 0), 1);
    device_create(lldev_class, NULL, MKDEV(dev_major, 0), NULL, "lldev", 0);

	// create nodes in list
	for (i = 0; i < NUM_ELEMENTS; i++)
	{
		node = kmalloc(sizeof(struct my_list), GFP_KERNEL);
		if (!node)
			return -ENOMEM;

		node->magic = jiffies + i;

		INIT_LIST_HEAD(&node->list);
		list_add_tail(&node->list, &head_node);
	}
	list_for_each_entry(temp, &head_node, list)
	{
		pr_info("lldev: Adding node with data = %lu\n", temp->magic);
		last_magic = temp->magic;
	}

    return 0;
}

static void __exit lldev_exit(void)
{
	struct my_list *cursor, *temp;
	pr_info("lldev: Module exit\n");

	// delete list elements
	list_for_each_entry_safe(cursor, temp, &head_node, list) {
		pr_info("lldev: Freeing node with data = %lu\n", cursor->magic);

		list_del(&cursor->list);
		kfree(cursor);
	}

    device_destroy(lldev_class, MKDEV(dev_major, 0));
    class_unregister(lldev_class);
    class_destroy(lldev_class);
    unregister_chrdev_region(MKDEV(dev_major, 0), MINORMASK);
}

static int lldev_open(struct inode *inode, struct file *file)
{
    printk("lldev: Device open\n");
    return 0;
}

static int lldev_release(struct inode *inode, struct file *file)
{
    printk("lldev: Device close\n");
    return 0;
}

static long lldev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    u_long answer;
    printk("lldev: Device ioctl\n");
    switch(cmd)
    {
        case DELL_NODE:
        {
            answer = lldev_help_del_node();
            if (copy_to_user((u_long *) arg, &answer, sizeof(answer)))
                pr_info("lldev: Error in IOCtl operation\n");
            else
                pr_info("lldev: Delete operation was successfully\n");
            break;
        }
        case SUM_NODES:
        {
            answer = lldev_help_sum();
            if (copy_to_user((u_long *) arg, &answer, sizeof(answer)))
                pr_info("lldev: Error in IOCtl operation\n");
            else
                pr_info("lldev: Summation operation was successfully\n");
            break;
        }
    }
    return 0;
}

static ssize_t lldev_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{
    int bytes_read = 0;
	pr_info("lldev: Device read\n");

    if (*offset != 0)
        return 0;

    bytes_read = snprintf(number_buffer, BUFFER_SIZE, "%lu\n", last_magic);
    if (copy_to_user(buf, number_buffer, bytes_read) != 0)
        return -EFAULT;

    *offset += bytes_read;
    return bytes_read;
}

static ssize_t lldev_write(struct file *file, const char __user *buf, size_t count, loff_t *offset)
{
    int err;
	u_long magic;
    size_t maxdatalen = 32, ncopied;
    uint8_t databuf[32];
	struct my_list *node;

    if (count < maxdatalen)
        maxdatalen = count;

    ncopied = copy_from_user(databuf, buf, maxdatalen);

    if (ncopied == 0)
        printk("Copied %zd bytes from the user\n", maxdatalen);
    else
        printk("Could't copy %zd bytes from the user\n", ncopied);

    databuf[maxdatalen] = 0;
    printk("Data from the user: %s", databuf);

	// Getting magic number provided
	err = kstrtoul(databuf, 10, &magic);
	if (err != 0)
		return (err == -ERANGE) ? -ERANGE : -EINVAL;

	// adding new node
	node = kmalloc(sizeof(struct my_list), GFP_KERNEL);
	if (!node)
		return -ENOMEM;

	node->magic = magic;

	INIT_LIST_HEAD(&node->list);
	list_add_tail(&node->list, &head_node);
	last_magic = node->magic;
	pr_info("lldev: Adding node with data = %lu\n", node->magic);

    return count;
}

static u_long lldev_help_sum(void)
{
    u_long sum = 0;
    struct my_list *temp;

	list_for_each_entry(temp, &head_node, list)
    {
        sum += temp->magic;
    }

    return sum;
}

static u_long lldev_help_del_node(void)
{
    struct my_list *temp, *tail = NULL;
    u_long res = 0;

	// delete last elements
	list_for_each_entry(temp, &head_node, list)
    {
        if (list_is_last(&temp->list, &head_node))
            tail = temp;
	}

    if (tail != NULL)
    {
    	list_del(&tail->list);
		kfree(tail);
        res = 1;
    }
    
    return res;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Felipe Jimenez <felipea.jimenezc@gmail.com>");

module_init(lldev_init);
module_exit(lldev_exit);
