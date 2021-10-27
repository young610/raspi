#include <linux/init.h> 
#include <linux/module.h> 
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/timer.h>
#include <linux/jiffies.h>

#define photo 17
#define ACTLED 42
#define LED 18

static unsigned int irq;
static unsigned int irqnum = 0;

static int off_timer = 2;
static int delay = 100;
static struct timer_list s_BlinkTimer;
static int AL=0;

dev_t dev = 0;
static struct class *dev_class;
struct kobject *kobj_ref;

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Derek Molloy");
MODULE_DESCRIPTION("A PHOTO/LED test drever for the RPi");
MODULE_VERSION("0.1");

static ssize_t delay_store(struct kobject *kobj,struct kobj_attribute *attr,const char *buf, size_t count)
{
    sscanf(buf,"%d",&delay);
	printk("Led delay value Write :%d \n",delay);
    return count;
}

static ssize_t off_timer_store(struct kobject *kobj,struct kobj_attribute *attr,const char *buf, size_t count)
{
    sscanf(buf,"%d",&off_timer);
	printk("Led delay value Write :%d \n",off_timer);
    return count;
}

struct kobj_attribute delay_attr= __ATTR(delay,0664, 0,delay_store); 
struct kobj_attribute off_timer_attr= __ATTR(off_timer,0664, 0,off_timer_store);

static void TimerHandler1(struct timer_list *unused) 
{
	AL = !AL;
	gpio_set_value(ACTLED,AL);
	mod_timer(&s_BlinkTimer, jiffies + msecs_to_jiffies(delay));	
	printk(KERN_INFO "photo_ intrrupt11111");	
}

static irq_handler_t photo_irq(unsigned int irq, void *dev_id, struct pt_regs *regs)
{
	if(gpio_get_value(photo)==1)
	{
		timer_setup(&s_BlinkTimer, TimerHandler1 , 0);
		mod_timer(&s_BlinkTimer, jiffies + msecs_to_jiffies(delay));
		printk(KERN_INFO "photo_ intrrupt  rising : %d\n",delay);
		irqnum++;
	}
	else
	{
		schedule_timeout_interruptible(off_timer*HZ); ////현재 자기자신한테 할당된 프로세스 시간을 반납하고 대기하는 방식
		del_timer(&s_BlinkTimer);
		printk(KERN_INFO "photo_ intrrupt  falling \n");
		irqnum++;
	}
	
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
	gpio_set_debounce(photo,200);
	gpio_export(photo,false);

	int a =0;

	irq = gpio_to_irq(photo);
	a = request_irq(irq,(irq_handler_t) photo_irq, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING ,"rpi_gpio_irqrising",NULL);
	
	printk(KERN_INFO "GPIO PHOTO mapped to IRQ : %d\n", irq);

	kobj_ref = kobject_create_and_add("photo_timer",kernel_kobj);
 
        if(sysfs_create_file(kobj_ref,&delay_attr.attr))
	{
                pr_err("Cannot create delay file......\n");        
    	}
		if(sysfs_create_file(kobj_ref,&off_timer_attr.attr))
	{
                pr_err("Cannot create delay file......\n");        
    	}
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
	kobject_put(kobj_ref); 
	sysfs_remove_file(kernel_kobj, &delay_attr.attr);
	sysfs_remove_file(kernel_kobj, &off_timer_attr.attr);
	class_destroy(dev_class);
	printk(KERN_INFO "######### PHOTO INTERRUPT BYE ########\n");
}

module_init(rpi_gpio_init);
module_exit(rpi_gpio_exit);
