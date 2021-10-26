obj-m := period.o
KDIR := /linux
default:
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -C $(KDIR) M=/home/ey/mmo/period modules

clean:
	make -C $(KDIR) M=/home/ey/mmo/period clean
