#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/io.h>

#define GPIO_BASE 0xFE200000
#define BLOCK_SIZE 4096
#define GPIO_LED 18
#define ACT_LED 10
#define GPFSEL18 (0x04/4)
#define GPSET18 (0x1C/4)
#define GPCLR18 (0x28/4)
#define GPFSEL42 (0x10/4)
#define GPSET42 (0x20/4)
#define GPCLR42 (0x2c/4)


volatile unsigned int * gpio_addr;

static void blinking(void){
	int i,j;
	for(i=0; i<5 ; i++){
		*(gpio_addr+GPSET18) |= 1 <<(GPIO_LED);
			mdelay(1000);
		*(gpio_addr+GPCLR18) |= 1 <<(GPIO_LED);
			mdelay(1000);
		for(j=0; j<5 ; j++){
			*(gpio_addr+GPSET42) |= 1 <<(ACT_LED);
				mdelay(100);
			*(gpio_addr+GPCLR42) |= 1 <<(ACT_LED);
				mdelay(100);
		}	
	}
}

static int hello_init(void) {
	printk("insmod : Hello, GPIO LED Module Test.....\n");
	gpio_addr = ioremap(GPIO_BASE, BLOCK_SIZE);
	*(gpio_addr+GPFSEL18) &= ~(6 << (((GPIO_LED)%10)*3));
	blinking(); // LED ON/OFF
return 0;
}

static void hello_exit(void) {
	iounmap(gpio_addr);
	printk("rmmod : Bye, GPIO LED Module Test......\n");
}

module_init(hello_init);
module_exit(hello_exit);
