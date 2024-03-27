# Hello world kernel module

EXTRA_CFLAGS = -Wall
obj-m = led_dev.o

#KDIR = /lib/modules/`uname -r`/build
KDIR = /home/thkhoa/work/bbb/kernelbuildscripts/KERNEL
#KDIR = /home/thkhoa/work/bbb
CROSS = /home/thkhoa/work/bbb/kernelbuildscripts/dl/gcc-8.5.0-nolibc/arm-linux-gnueabi/bin/arm-linux-gnueabi-
all:
#	make -C $(KDIR) M=`pwd` modules
	make ARCH=arm CROSS_COMPILE=$(CROSS) -C $(KDIR) M=`pwd` modules
#	make ARCH=arm CROSS_COMPILE=/home/thkhoa/work/bbb/gcc-11.3.0-nolibc/arm-linux-gnueabi/bin/arm-linux-gnueabi- -C $(KDIR) M=`pwd` modules

clean:
#	make -C $(KDIR) M=`pwd` clean
#	make ARCH=arm CROSS_COMPILE=/home/thkhoa/work/bbb/gcc-11.3.0-nolibc/arm-linux-gnueabi/bin/arm-linux-gnueabi- -C $(KDIR) M=`pwd` clean
	make ARCH=arm CROSS_COMPILE=$(CROSS) -C $(KDIR) M=`pwd` clean
	rm -rf app

help:
	make ARCH=arm CROSS_COMPILE=$(CROSS) -C $(KDIR) M=`pwd` help

app:
	arm-linux-gnueabihf-gcc -o app app.c
