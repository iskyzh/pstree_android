INSTALL_PATH=/data/local

all: run

rmmod: FORCE
	adb shell "cd /data/local && rmmod android_module.ko"

module: FORCE
	cd android_module && ./compile.sh

push: ptree_test ptree exec_test
	@echo "Pushing executables..."
	@adb push ./ptree_test/obj/local/$(ARCH)/ptree_test $(INSTALL_PATH)
	@adb push ./ptree/obj/local/$(ARCH)/ptree $(INSTALL_PATH)
	@adb push ./exec_test/obj/local/$(ARCH)/exec_test $(INSTALL_PATH)

install: module rmmod
	@echo "Pushing kernel module..."
	@adb push ./android_module/android_module.ko $(INSTALL_PATH)
	@echo "Installing kernel module..."
	@adb shell "cd $(INSTALL_PATH) && insmod android_module.ko"

test: install push
	@echo "Running tests..."
	adb shell "cd $(INSTALL_PATH) && ./ptree_test"

run: install push
	adb shell "cd $(INSTALL_PATH) && ./exec_test"

run_kitchen: kitchen
	@adb push ./kitchen/obj/local/$(ARCH)/kitchen $(INSTALL_PATH)
	adb shell "cd $(INSTALL_PATH) && ./kitchen"

run_ptree: install push
	adb shell "cd $(INSTALL_PATH) && ./ptree"

ptree_test: FORCE
	cd ptree_test && ndk-build

ptree: FORCE
	cd ptree && ndk-build

exec_test: FORCE
	cd exec_test && ndk-build

kitchen: FORCE
	cd kitchen && ndk-build

clean:
	cd android_module && make clean

FORCE:

.PHONY: FORCE clean
