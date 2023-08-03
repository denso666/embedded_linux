#ifndef RTC_DS3231
#define RTC_DS3231

enum ds_type
{
	ds_3231,
	last_ds_type
};

#define DS3231_REG_SECS		0x00
#define DS3231_REG_MIN		0x01
#define DS3231_REG_HOUR		0x02
#define DS3231_REG_WDAY		0x03
#define DS3231_REG_MDAY		0x04
#define DS3231_REG_MONTH	0x05
#define DS3231_BIT_CENTURY	0x80
#define DS3231_REG_YEAR		0x06
#define DS3231_REG_CONTROL	0x0e
#define DS3231_BIT_nEOSC	0x80
#define DS3231_BIT_BBSQW	0x40
#define DS3231_REG_STATUS	0x0f
#define DS3231_BIT_OSF		0x80

struct ds3231
{
	enum ds_type type;
	struct device *dev;
	struct regmap *regmap;
	const char *name;
	struct rtc_device *rtc;
};

struct chip_desc
{
	unsigned	alarm:1;
	u8			offset;
	u8			century_reg;
	u8			century_enable_bit;
	u8			century_bit;
	u8			bbsqi_bit;
	const struct rtc_class_ops *rtc_ops;
};

#endif
