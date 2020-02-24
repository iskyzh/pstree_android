all: run

rmmod: FORCE
	adb shell "cd /data/local && rmmod android_module.ko"

module: FORCE
	cd android_module && ./compile.sh

run: module build rmmod
	adb push ./android_module/android_module.ko /data/local
	adb push ./syscall_program/obj/local/$(ARCH)/ptree /data/local
	adb shell "cd /data/local && insmod android_module.ko && ./ptree"

build: FORCE
	cd syscall_program && ndk-build

FORCE:

.PHONY: FORCE
