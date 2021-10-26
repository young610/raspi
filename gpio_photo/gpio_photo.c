#include <linux/init.h> 
#include <linux/module.h> 
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>

#define gpioLED 18
#define gpiophoto 17
#define ACTLED 42

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Derek Molloy");
MODULE_DESCRIPTION("A PHOTO/LED test drever for the RPi");
MODULE_VERSION("0.1");

static unsigned int irqris;
static unsigned int numris = 0;
static bool ledOn =0;

static irq_handler_t rpi_gpio_irqrising(unsigned int irq, void *dev_id, struct pt_regs *regs)
{
	ledOn = !ledOn;
	gpio_set_value(gpioLED,ledOn);
	gpio_set_value(ACTLED,ledOn);
	printk(KERN_INFO "INTERRUPT! => ACT,GPIOLED ON~~~ [RISING : %d ]",gpio_get_value(gpiophoto));
	numris++;
	return (irq_handler_t) IRQ_HANDLED;		
}

static int __init rpi_gpio_init(void)
{
	int a =0;

	printk(KERN_INFO "@@@@@ PHOTO INTERRUPT START @@@@@\n");
	
	gpio_request(gpioLED,"sysfs");
	gpio_direction_output(gpioLED,1);
	gpio_set_value(gpioLED,0);
	gpio_export(gpioLED,false);

	gpio_request(ACTLED,"sysfs");
	gpio_direction_output(ACTLED,1);
	gpio_set_value(ACTLED,0);
	gpio_export(ACTLED,false);

	gpio_request(gpiophoto,"sysfs");
	gpio_direction_input(gpiophoto);
	gpio_get_value(gpiophoto);
	gpio_export(gpiophoto,false);

	printk(KERN_INFO "currently PHOTO value: %d\n",gpio_get_value(gpiophoto));

	irqris = gpio_to_irq(gpiophoto);
	printk(KERN_INFO "GPIO PHOTO mapped to IRQ : %d\n", irqris);
	
	a = request_irq(irqris,(irq_handler_t) rpi_gpio_irqrising, IRQF_TRIGGER_RISING,"rpi_gpio_irqrising",NULL);
	
	printk(KERN_INFO "irqris IRQ result: %d\n",a);

	return 0;
}


static void __exit rpi_gpio_exit(void) {
	printk(KERN_INFO "currently PHOTO value: %d\n",gpio_get_value(gpiophoto));
	printk(KERN_INFO "NUMBER RISING %d TIMES\n",numris);
	gpio_set_value(gpioLED,0);
	gpio_unexport(gpioLED);
	gpio_set_value(ACTLED,0);
	gpio_unexport(ACTLED);
	free_irq(irqris, NULL);
	gpio_unexport(gpiophoto);
	gpio_free(gpioLED);
	gpio_free(ACTLED);
	gpio_free(gpiophoto);
	printk(KERN_INFO "===== GOOD BYE PHOTO INTERRUPT =====\n");
}

module_init(rpi_gpio_init);
module_exit(rpi_gpio_exit);









