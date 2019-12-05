#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/sysfs.h>
#include <linux/delay.h>

/*Conversion register*/
#define ADS1115_CONV_REG 0x00
/*Configuration register*/
#define ADS1115_CONFIG_REG 0x01

/*FSR=±2.048 V, LSB SIZE=62.5 μV, MODE=CONTINUOS_CONVERSION*/
#define ADS1115_INIT_CONV 0xc483  

/*To print conversion in Binary */
#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 

struct adc_ads1115{
    struct i2c_client *client;
};

static ssize_t ads1115_v_read(struct device *dev,
			       struct device_attribute *attr,
			       char *buf)
{
    struct adc_ads1115 *adc = dev_get_drvdata(dev);
    struct i2c_client *client = adc->client;
    __s32 ret;

	
	printk(KERN_DEBUG "CONFIG REG BEFORE : 0x%x \n", i2c_smbus_read_word_data(client, ADS1115_CONFIG_REG));
	ret = i2c_smbus_write_word_data(client, ADS1115_CONFIG_REG, ADS1115_INIT_CONV);
    mdelay(10);
	if (ret<0)
	{
		printk(KERN_DEBUG "ERROR WRITING CONFIG: %x \n", ret);
	}
	printk(KERN_DEBUG "CONFIG REG AFTER : 0x%x \n", i2c_smbus_read_word_data(client, ADS1115_CONFIG_REG));
	mdelay(10);
	ret = i2c_smbus_read_word_data(client, ADS1115_CONV_REG);
    printk(KERN_DEBUG "Voltage is: 0x%x \n", i2c_smbus_read_word_data(client, ADS1115_CONV_REG));
    if(ret < 0){
        return ret;
    }
	/*ret = ret * 62.5 / 1000000 (hex)*/
	ret = ((ret * 0x3e) / 0xf4240);

    *buf = (__u16)ret;
    
    return sprintf(buf, "Actual voltage is: %d \n", ret);

}

static ssize_t ads1115_config(struct device *dev,
						struct device_attribute *attr,
						char *buf)
{
	struct adc_ads1115 *adc = dev_get_drvdata(dev);
	struct i2c_client *client = adc->client;
	__s32 ret;

	ret = i2c_smbus_write_word_data(client, ADS1115_CONFIG_REG, ADS1115_INIT_CONV);
	if(ret < 0){
		return ret;
	}

	return sprintf(buf, "Initial Configuration is: %x",  ADS1115_INIT_CONV);
}


static DEVICE_ATTR(voltage,	S_IRUGO, ads1115_v_read, NULL);
static DEVICE_ATTR(initconfig, S_IRUGO, ads1115_config, NULL);

static struct attribute *ads1115_attributes[] = {

	&dev_attr_voltage.attr,
	&dev_attr_initconfig.attr,
	NULL,
};


static const struct attribute_group ads1115_attr_group = {
	.attrs	= ads1115_attributes,
};


static int ads1115_i2c_probe(struct i2c_client *client,
			    const struct i2c_device_id *id)
{   
    printk(KERN_ERR  "TOM ADC DRIVER PROBED \n");
	struct kobject *kobj = &client->dev.kobj;
	struct adc_ads1115 *ads;
	int err;
    
	// ci pensa il kernel a fare la freee
	// GFP_KERNEL è il tipo di memoria (general purpose)
	// GFP_ATOMIC memoria sotto interrupt del kernel
	ads = devm_kzalloc(&client->dev, sizeof(*ads), GFP_KERNEL);
	if (!ads) {
		dev_err(&client->dev, "failed to allocate memory\n");
		return -ENOMEM;
	}

	i2c_set_clientdata(client, ads);

	/* Fill in LPS25H data structure */
	ads->client = client;

	err = sysfs_create_group(kobj, &ads1115_attr_group);
	if (err) {
		dev_err(&client->dev, "failed to create sysfs group\n");
		return err;
	}

	// /* Configure sensor */
	// err = lps25h_enable(lps);
	// if (err) {
	// 	dev_err(&client->dev, "failed to issue enable\n");
	// 	goto err_enable;
	// }

	return 0;

err_enable:
	sysfs_remove_group(kobj, &ads1115_attr_group);
	return err;
}


static int ads1115_i2c_remove(struct i2c_client *client)
{
	struct adc_ads1115 *ads = i2c_get_clientdata(client);
	struct kobject *kobj = &client->dev.kobj;

	// lps25h_disable(lps);
	sysfs_remove_group(kobj, &ads1115_attr_group);

	return 0;
}


#ifdef CONFIG_OF
static const struct of_device_id ads1115_of_match[] = {
	{ .compatible = "ti,ads1115-volt" },
	{},
};
MODULE_DEVICE_TABLE(of, ads1115_of_match);
#else
#define ads1115_of_match NULL
#endif

static struct i2c_driver ads1115_driver = {
	.driver = {
		.name = "ads1115-i2c",
		.of_match_table = of_match_ptr(ads1115_of_match),
	},
	.probe = ads1115_i2c_probe,
	.remove = ads1115_i2c_remove,
};



static int __init sample_i2c_init_driver(void)
{
	printk(KERN_ERR  "TOM ADC DRIVER init \n");
	return i2c_add_driver(&ads1115_driver);
}

static void __exit sample_i2c_exit_driver(void)
{
	printk(KERN_ERR  "TOM ADC DRIVER removed \n");
	i2c_del_driver(&ads1115_driver);
}

module_init(sample_i2c_init_driver);
module_exit(sample_i2c_exit_driver);


MODULE_AUTHOR("Tommaso Merciai <tommaso.merciai@seco.com");
MODULE_DESCRIPTION("Driver for ADS1115 adc sensor");
MODULE_LICENSE("GPL v2");