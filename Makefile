all: run

rmmod:
	adb shell "cd /data/local && rmmod android_module.ko"

run: build rmmod
	adb push /Volumes/OS-Working/android_module/android_module.ko /data/local
	adb push /Volumes/OS-Working/syscall_program/obj/local/x86_64/ptree /data/local
	adb shell "cd /data/local && insmod android_module.ko && ./ptree"

build: FORCE
	cd syscall_program && ndk-build

FORCE:

.PHONY: FORCE
