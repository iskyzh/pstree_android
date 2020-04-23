obj-m += ptree_module.o
KID ?= ../goldfish
ARCH=arm
CROSS_COMPILE ?= arm-linux-androideabi-
CC=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)ld

all: gen.h
	make -C $(KID) M=$(shell pwd) modules

clean:
	make -C $(KID) M=$(shell pwd) clean

gen.h: gen.sh
	@./gen.sh
