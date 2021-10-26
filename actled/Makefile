obj-m := actled.o
KDIR := /linux
default:
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -C $(KDIR) M=/home/ey/mmo/actled modules

clean:
	make -C $(KDIR) M=/home/ey/mmo/actled clean
