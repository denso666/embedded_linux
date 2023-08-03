/**
 * @file external-led.c
 * @author Felipe Jimenez <felipea.jimenezc@gmail.com>
 * @date 19-07-2023
 */
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/of_address.h>
#include <linux/gpio/consumer.h>
#include <linux/platform_device.h>
#include <linux/proc_fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Felipe Jimenez <felipea.jimenezc@gmail.com");

#define NUM_LEDS 4
static const char *led_labels[] = {
    "ext-0",
    "ext-1",
    "ext-2",
    "ext-3"
};
static const char *gpio_labels[] = {
    "ext-0-gpio",
    "ext-1-gpio",
    "ext-2-gpio",
    "ext-3-gpio"
};
struct private_gpio {
    struct gpio_desc *leds[NUM_LEDS];
    struct proc_dir_entry *proc_files[NUM_LEDS];
    char data_trans[NUM_LEDS][32];
};
static struct private_gpio priv_gpio; 

static int externalled_probe(struct platform_device*);
static int externalled_remove(struct platform_device*);
static ssize_t externalled_write0(struct file*, const char __user*, size_t, loff_t*);
static ssize_t externalled_write1(struct file*, const char __user*, size_t, loff_t*);
static ssize_t externalled_write2(struct file*, const char __user*, size_t, loff_t*);
static ssize_t externalled_write3(struct file*, const char __user*, size_t, loff_t*);

static struct of_device_id of_externalled_match[] = {
    { .compatible = "dle,externalled" },
    { /* end */ },
};
MODULE_DEVICE_TABLE(of, of_externalled_match);
static struct platform_driver externalled_driver = {
	.probe    = externalled_probe,
	.remove   = externalled_remove,
	.driver   =
    {
		.name = "externalled",
		.of_match_table = of_externalled_match,
	},
};
static struct proc_ops externalled_ops[4] = {
    { .proc_write = externalled_write0, },
    { .proc_write = externalled_write1, },
    { .proc_write = externalled_write2, },
    { .proc_write = externalled_write3, },
};

static ssize_t externalled_write(struct file *File, const char __user *buf, size_t count, loff_t *offs, unsigned int id)
{
    pr_info("externalled: ext-led-%u - write was called\n", id);
    copy_from_user(priv_gpio.data_trans[id], buf, count);
    gpiod_set_value(
        priv_gpio.leds[id],
        ((priv_gpio.data_trans[id][0] == '0') ? 0 : 1)
    );

    return count;
}
static ssize_t externalled_write0(struct file *File, const char __user *buf, size_t count, loff_t *offs)
{
    return externalled_write(File, buf, count, offs, 0);
}
static ssize_t externalled_write1(struct file *File, const char __user *buf, size_t count, loff_t *offs)
{
    return externalled_write(File, buf, count, offs, 1);
}
static ssize_t externalled_write2(struct file *File, const char __user *buf, size_t count, loff_t *offs)
{
    return externalled_write(File, buf, count, offs, 2);
}
static ssize_t externalled_write3(struct file *File, const char __user *buf, size_t count, loff_t *offs)
{
    return externalled_write(File, buf, count, offs, 3);
}

static int externalled_probe(struct platform_device *pdev)
{
    int i;
    struct device *dev = &pdev->dev;
    pr_info("externalled: probe\n");

    for (i = 0; i<NUM_LEDS; i++)
    {
        if (device_property_present(dev, gpio_labels[i]) == 0)
        {
            pr_err("externalled: %s property not found\n", gpio_labels[i]);            
            return -1;
        }
        else
            pr_info("externalled: %s property found\n", gpio_labels[i]);

        priv_gpio.leds[i] = gpiod_get(dev, led_labels[i], GPIOD_OUT_LOW);
        if (IS_ERR(priv_gpio.leds[i]))
        {
            pr_err("externalled: %s gpio not found\n", led_labels[i]);
            return -1;
        }
        else
            pr_info("externalled: %s gpio found\n", led_labels[i]);

        priv_gpio.proc_files[i] = proc_create(led_labels[i], 0666, NULL, &externalled_ops[i]);
        if (priv_gpio.proc_files[i] == NULL)
        {
            pr_err("externalled: proc %s error", led_labels[i]);
            gpiod_put(priv_gpio.leds[i]);
            return -ENOMEM;
        }
        else
            pr_info("externalled: proc %s", led_labels[i]);
    }
    return 0;
}
static int externalled_remove(struct platform_device*)
{
    int i = 0;
	pr_info("externalled: remove\n");
    for (i = 0; i<NUM_LEDS; i++)
    {
        gpiod_put(priv_gpio.leds[i]);
        proc_remove(priv_gpio.proc_files[i]);
    }
	return 0;
}
static int __init externalled_init(void)
{
	pr_info("externalled: init\n");
	if (platform_driver_register(&externalled_driver))
    {
    	pr_info("externalled: error on init process\n");
		return -1;
	}
	return 0;
}
static void __exit externalled_exit(void)
{
	pr_info("externalled: exit\n");
	platform_driver_unregister(&externalled_driver);
}

module_init(externalled_init);
module_exit(externalled_exit);
