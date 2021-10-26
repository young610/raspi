obj-m := syss.o
KDIR := /linux
default:
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -C $(KDIR) M=/home/ey/mmo/syss modules

clean:
	make -C $(KDIR) M=/home/ey/mmo/syss clean
