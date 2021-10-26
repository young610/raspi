#include <linux/init.h> 
#include <linux/module.h> 
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/timer.h>

#define photo 17
#define ACTLED 42
#define LED 18

static unsigned int irq;
static unsigned int irqnum = 0;
volatile int delay = 100;

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Derek Molloy");
MODULE_DESCRIPTION("A PHOTO/LED test drever for the RPi");
MODULE_VERSION("0.1");

static irq_handler_t photo_irq(unsigned int irq, void *dev_id, struct pt_regs *regs)
{
	printk(KERN_INFO "photo_ intrrupt");

		AL = !AL;
		gpio_set_value(ACTLED,AL);
		mdelay(delay);

	irqnum++;

	return (irq_handler_t) IRQ_HANDLED;		
}

static int __init rpi_gpio_init(void)
{	
	printk(KERN_INFO "@@@@@@@@@ PHOTO INTERRUPT START @@@@@@@@\n");

	gpio_request(ACTLED,"actled");
	gpio_direction_output(ACTLED,0);
	gpio_export(ACTLED,false);

	gpio_request(LED,"led");
	gpio_direction_output(LED,0);
	gpio_export(LED,false);

	gpio_request(photo,"photo");
	gpio_direction_input(photo);
	gpio_get_value(photo);
	gpio_export(photo,false);

	int a =0;

	irq = gpio_to_irq(photo);
	a = request_irq(irq,(irq_handler_t) photo_irq, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING ,"rpi_gpio_irqrising",NULL);
	
	printk(KERN_INFO "GPIO PHOTO mapped to IRQ : %d\n", irq);
	return 0;
}

static void __exit rpi_gpio_exit(void) {
	printk(KERN_INFO "currently PHOTO value: %d\n",gpio_get_value(photo));
	printk(KERN_INFO "RISING NUMBER %d TIMES\n",irqnum);
	gpio_set_value(ACTLED,0);
	gpio_unexport(ACTLED);
	gpio_set_value(LED,0);
	gpio_unexport(LED);
	free_irq(irq, NULL);
	gpio_unexport(photo);
	gpio_free(ACTLED);
	gpio_free(LED);
	gpio_free(photo);
	printk(KERN_INFO "######### PHOTO INTERRUPT BYE ########\n");
}

module_init(rpi_gpio_init);
module_exit(rpi_gpio_exit);
