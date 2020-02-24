obj-m += android_module.o
KID ?= ../goldfish
CROSS_COMPILE ?= x86_64-linux-android-
CC=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)ld

all:
	make -C $(KID) M=$(shell pwd) modules

clean:
	make -C $(KID) M=$(shell pwd) clean
