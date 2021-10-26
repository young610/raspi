#include <linux/init.h> 
#include <linux/module.h> 
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/timer.h>

#define gpiophoto 17
#define ACTLED 42
#define GPIOLED 18

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Derek Molloy");
MODULE_DESCRIPTION("A PHOTO/LED test drever for the RPi");
MODULE_VERSION("0.1");

static unsigned int irqris;
static unsigned int numris = 0;
static unsigned int numfal = 0;
volatile int delay = 100;
static int s_BlinkPeriod = 200;

dev_t dev = 0;
static struct class *dev_class;
struct kobject *kobj_ref;
static struct timer_list s_BlinkTimer;

/****       delay  show store    *****/
static ssize_t sysfs_show(struct kobject *kobj,struct kobj_attribute *attr, char *buf)
{
        printk("Led delay value Read : %d \n",delay);
        return sprintf(buf, "%d", delay);
}

static ssize_t sysfs_store(struct kobject *kobj,struct kobj_attribute *attr,const char *buf, size_t count)
{
        sscanf(buf,"%d",&delay);
	printk("Led delay value Write :%d \n",delay);
        return count;
}

/****     off_timer  show store    *****/
static ssize_t set_period_callback(struct device* dev, struct device_attribute* attr, const char* buf, size_t count) 
{
	long period_value = 0;
	
	if (kstrtol(buf, 10, &period_value) < 0)
		return -EINVAL;
	if (period_value < 10)	
		return -EINVAL;

	s_BlinkPeriod = period_value;
	return count;
}

struct kobj_attribute delay_attr= __ATTR(delay,0664, sysfs_show,sysfs_store); 
struct kobj_attribute off_attr= __ATTR(off_timer,0664, 0,set_period_callback);

//******** IRQ HANDLER   *******//
static irq_handler_t rpi_gpio_irq(unsigned int irq, void *dev_id, struct pt_regs *regs)
{
	
	if(gpio_get_value(gpiophoto)==1)
	{
		timer_setup(&s_BlinkTimer, TimerHandler, 0);
		printk(KERN_INFO "__ACTLED OFF__(delay %d) [PHOTO : %d ]",delay ,gpio_get_value(gpiophoto));
		numris++; 
	}
	else
	{
		printk(KERN_INFO "==ACTLED ON==(delay %d) [PHOTO : %d ]",delay ,gpio_get_value(gpiophoto));
		numfal++;
		mod_timer(&s_BlinkTimer,jiffies + msecs_to_jiffies(s_BlinkPeriod));
	}

	return (irq_handler_t) IRQ_HANDLED;		
}

static void TimerHandler(unsigned long unused)
{
	int i;
	static bool AL = 0;
	
		AL = !AL;
		gpio_set_value(ACTLED,AL);
		mdelay(delay);
	
}

static int __init rpi_gpio_init(void)
{
	int a =0;

	printk(KERN_INFO "@@@@@@@@@ PHOTO INTERRUPT START @@@@@@@@\n");

	gpio_request(ACTLED,"sysfs");
	gpio_direction_output(ACTLED,0);
	gpio_export(ACTLED,false);

	gpio_request(gpiophoto,"sysfs");
	gpio_direction_input(gpiophoto);
	gpio_get_value(gpiophoto);
	gpio_export(gpiophoto,false);

        kobj_ref = kobject_create_and_add("raspi_photo",kernel_kobj);
 
        if(sysfs_create_file(kobj_ref,&delay_attr.attr))
	{
                pr_err("Cannot create delay file......\n");        
    	}
	
	if(sysfs_create_file(kobj_ref,&off_attr.attr))
	{
                pr_err("Cannot create off_timer file......\n");        
    	}

        printk("Device Driver Insert...Done!!!\n");

	printk(KERN_INFO "currently PHOTO value: %d\n",gpio_get_value(gpiophoto));

	irqris = gpio_to_irq(gpiophoto);
	printk(KERN_INFO "GPIO PHOTO mapped to IRQ : %d\n", irqris);
	
	a = request_irq(irqris,(irq_handler_t) rpi_gpio_irq, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING ,"rpi_gpio_irq",NULL);
	
	printk(KERN_INFO "irqris IRQ result: %d\n",a);
	
	return 0;
}

static void __exit rpi_gpio_exit(void) {
	printk(KERN_INFO "currently PHOTO value: %d\n",gpio_get_value(gpiophoto));
	printk(KERN_INFO "RISING NUMBER %d TIMES\n",numris);
	printk(KERN_INFO "FALLING NUMBER %d TIMES\n",numfal);
	gpio_set_value(ACTLED,0);
	gpio_unexport(ACTLED);
	free_irq(irqris, NULL);
	gpio_unexport(gpiophoto);
	gpio_free(ACTLED);
	gpio_free(gpiophoto);
	kobject_put(kobj_ref); 
	sysfs_remove_file(kernel_kobj, &delay_attr.attr);
	sysfs_remove_file(kernel_kobj, &off_attr.attr);
        device_destroy(dev_class,dev);
        class_destroy(dev_class);
	del_timer(&s_BlinkTimer);
        printk("Device Driver Remove...Done!!!\n");
	printk(KERN_INFO "######### PHOTO INTERRUPT BYE ########\n");
}

module_init(rpi_gpio_init);
module_exit(rpi_gpio_exit);
