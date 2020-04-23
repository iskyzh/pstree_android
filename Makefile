INSTALL_PATH=/data/local
BBC_PARAMETER=2 4 41 10
ARCH=armeabi

all: test run

ptree_module: FORCE
	cd ptree_module && make

ptree_test: FORCE
	cd ptree_test && ndk-build

ptree: FORCE
	cd ptree && ndk-build

exec_test: FORCE
	cd exec_test && ndk-build

burger_buddies: FORCE
	cd burger_buddies && ndk-build

push: ptree_test ptree exec_test
	@echo "Pushing executables..."
	@adb push ./ptree_test/obj/local/$(ARCH)/ptree_test $(INSTALL_PATH)
	@adb push ./ptree/obj/local/$(ARCH)/ptree $(INSTALL_PATH)
	@adb push ./exec_test/obj/local/$(ARCH)/exec_test $(INSTALL_PATH)

rmmod: FORCE
	adb shell "cd /data/local && rmmod ptree_module.ko"

install: ptree_module rmmod
	@echo "Pushing kernel module..."
	@adb push ./ptree_module/ptree_module.ko $(INSTALL_PATH)
	@echo "Installing kernel module..."
	@adb shell "cd $(INSTALL_PATH) && insmod ptree_module.ko"

test: install push
	@echo "Running tests..."
	adb shell "cd $(INSTALL_PATH) && ./ptree_test"

run: install push
	adb shell "cd $(INSTALL_PATH) && ./exec_test"

run_bbc: burger_buddies
	@adb push ./burger_buddies/obj/local/$(ARCH)/BBC $(INSTALL_PATH)
	adb shell "cd $(INSTALL_PATH) && ./BBC $(BBC_PARAMETER)" | tee ./burger_buddies/result/log.txt

run_ptree: install push
	adb shell "cd $(INSTALL_PATH) && ./ptree"

clean:
	cd ptree_module && make clean
	cd exec_test && ndk-build clean
	cd burger_buddies && ndk-build clean
	cd ptree_test && ndk-build clean
	cd ptree && ndk-build clean
	
FORCE:

.PHONY: FORCE clean
