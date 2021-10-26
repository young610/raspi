obj-m := gpio_photo.o
KDIR := /linux
default:
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -C $(KDIR) M=/home/ey/mmo/gpio_photo modules

clean:
	make -C $(KDIR) M=/home/ey/mmo/gpio_photo clean
