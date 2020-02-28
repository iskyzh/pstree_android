all: run

rmmod: FORCE
	adb shell "cd /data/local && rmmod android_module.ko"

module: FORCE
	cd android_module && ./compile.sh

run: module build test rmmod
	adb push ./android_module/android_module.ko /data/local
	adb push ./syscall_program/obj/local/$(ARCH)/ptree /data/local
	adb push ./exec_test/obj/local/$(ARCH)/exec_test /data/local
	adb shell "cd /data/local && insmod android_module.ko && ./exec_test"

build: FORCE
	cd syscall_program && ndk-build

test: FORCE
	cd exec_test && ndk-build

clean:
	cd android_module && make clean

FORCE:

.PHONY: FORCE clean
