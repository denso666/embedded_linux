/*
 * @brief RTC DS3231 driver
 * @author Felipe Jimenez <felipea.jimenezc@gmail.com>
 * @date 27/07/2023
 */

#include <linux/bcd.h>
#include <linux/i2c.h>
#include <linux/rtc.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/regmap.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include "./rtc-ds3231.h"

static const struct chip_desc chips[last_ds_type];

static int ds3231_get_time(struct device *dev, struct rtc_time *t)
{
	int	tmp, ret;
	u8 regs[7];
	struct ds3231 *ds3231 = NULL;
	const struct chip_desc *chip = NULL;

	pr_info("ds_get_time\n");
	ds3231 = dev_get_drvdata(dev);
	chip = &chips[ds3231->type];

	ret = regmap_bulk_read(ds3231->regmap, chip->offset, regs, sizeof(regs));
	if (ret)
	{
		pr_info("%s error %d\n", "read", ret);
		return ret;
	}

	tmp = regs[DS3231_REG_SECS];

	t->tm_sec = bcd2bin(regs[DS3231_REG_SECS] & 0x7f);
	t->tm_min = bcd2bin(regs[DS3231_REG_MIN] & 0x7f);
	tmp = regs[DS3231_REG_HOUR] & 0x3f;
	t->tm_hour = bcd2bin(tmp);
	t->tm_wday = bcd2bin(regs[DS3231_REG_WDAY] & 0x07) - 1;
	t->tm_mday = bcd2bin(regs[DS3231_REG_MDAY] & 0x3f);
	tmp = regs[DS3231_REG_MONTH] & 0x1f;
	t->tm_mon = bcd2bin(tmp) - 1;
	t->tm_year = bcd2bin(regs[DS3231_REG_YEAR]) + 100;

	if (regs[chip->century_reg] & chip->century_bit && IS_ENABLED(CONFIG_RTC_DRV_DS1307_CENTURY))
		t->tm_year += 100;

	return 0;
}
static int ds3231_set_time(struct device *dev, struct rtc_time *t)
{
	int	result;
	int	tmp;
	u8 regs[7];
	struct ds3231 *ds3231 = dev_get_drvdata(dev);
	const struct chip_desc *chip = &chips[ds3231->type];

	pr_info("ds_set_time\n");

	if (t->tm_year < 100)
		return -EINVAL;

	regs[DS3231_REG_SECS] = bin2bcd(t->tm_sec);
	regs[DS3231_REG_MIN] = bin2bcd(t->tm_min);
	regs[DS3231_REG_HOUR] = bin2bcd(t->tm_hour);
	regs[DS3231_REG_WDAY] = bin2bcd(t->tm_wday + 1);
	regs[DS3231_REG_MDAY] = bin2bcd(t->tm_mday);
	regs[DS3231_REG_MONTH] = bin2bcd(t->tm_mon + 1);

	tmp = t->tm_year - 100;
	regs[DS3231_REG_YEAR] = bin2bcd(tmp);

	if (chip->century_enable_bit)
		regs[chip->century_reg] |= chip->century_enable_bit;
	if (t->tm_year > 199 && chip->century_bit)
		regs[chip->century_reg] |= chip->century_bit;


	result = regmap_bulk_write(ds3231->regmap, chip->offset, regs, sizeof(regs));
	if (result)
	{
		pr_info("%s error %d\n", "write", result);
		return result;
	}

	return 0;
}

static const struct chip_desc chips[last_ds_type] = {
	[ds_3231] = {
		.alarm			= 1,
		.century_reg	= DS3231_REG_MONTH,
		.century_bit	= DS3231_BIT_CENTURY,
		.bbsqi_bit		= DS3231_BIT_BBSQW,
	},
};

static const struct i2c_device_id ds3231_id[] = {
	{ "ds3231", ds_3231 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, ds3231_id);
static const struct of_device_id ds3231_of_match[] = {
	{ .compatible = "maxim,ds3231" },
	{ }
};
MODULE_DEVICE_TABLE(of, ds3231_of_match);
static const struct rtc_class_ops ds3231_rtc_ops = {
	.read_time	= ds3231_get_time,
	.set_time	= ds3231_set_time,
	// .read_time	= ds3231_read_time,
	// .set_time	= ds3231_set_time,
};
static const struct regmap_config regmap_config = {
	.reg_bits = 8,
	.val_bits = 8,
};

static int ds3231_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct ds3231 *ds3231;
	int	err = -ENODEV;
	const struct chip_desc *chip;
	unsigned char regs[8];

	ds3231 = devm_kzalloc(&client->dev, sizeof(struct ds3231), GFP_KERNEL);
	if (!ds3231)
	{
		pr_info("Cannot allocate memory\n");
		return -ENOMEM;
	}

	dev_set_drvdata(&client->dev, ds3231);
	ds3231->dev = &client->dev;
	ds3231->name = client->name;

	ds3231->regmap = devm_regmap_init_i2c(client, &regmap_config);
	if (IS_ERR(ds3231->regmap))
	{
		pr_info("regmap allocation failed\n");
		return PTR_ERR(ds3231->regmap);
	}

	i2c_set_clientdata(client, ds3231);

	ds3231->type = (enum ds_type)of_device_get_match_data(&client->dev);
	chip = &chips[ds3231->type];

	err = regmap_bulk_read(ds3231->regmap, DS3231_REG_CONTROL, regs, 2);
	if (err)
	{
		pr_info("read error %d\n", err);
		goto exit;
	}

	if (regs[0] & DS3231_BIT_nEOSC)
		regs[0] &= ~DS3231_BIT_nEOSC;

	regmap_write(ds3231->regmap, DS3231_REG_CONTROL, regs[0]);
	if (regs[1] & DS3231_BIT_OSF)
	{
		regmap_write(ds3231->regmap, DS3231_REG_STATUS, regs[1] & ~DS3231_BIT_OSF);
		pr_info("Set Time!\n");
	}

	err = regmap_bulk_read(ds3231->regmap, chip->offset, regs, sizeof(regs));
	if (err)
		goto exit;

	ds3231->rtc = devm_rtc_allocate_device(ds3231->dev);
	if (IS_ERR(ds3231->rtc))
		return PTR_ERR(ds3231->rtc);

	ds3231->rtc->ops = &ds3231_rtc_ops;

	err = rtc_register_device(ds3231->rtc);
	if (err)
		return err;

	return 0;

exit:
	return err;
}

static struct i2c_driver ds3231_driver = {
	.driver = {
		.name	= "rtc-ds3231",
		.of_match_table = of_match_ptr(ds3231_of_match),
	},
	.probe		= ds3231_probe,
	.id_table	= ds3231_id,
};

module_i2c_driver(ds3231_driver);

MODULE_DESCRIPTION("RTC driver for DS3231");
MODULE_LICENSE("GPL");
