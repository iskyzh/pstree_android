INSTALL_PATH=/data/local
BBC_PARAMETER=2 4 41 10
ARCH=armeabi
SHELL:=/bin/bash
TESTCASE_COLOR=\033[1;34m
NO_COLOR=\033[0m

all: test_ptree run

build_ptree_module: FORCE
	@echo -e "$(TESTCASE_COLOR)building kernel module...$(NO_COLOR)"
	cd ptree_module && make

build_ptree_test: FORCE
	@echo -e "$(TESTCASE_COLOR)building ptree test...$(NO_COLOR)"
	cd ptree_test && ndk-build

build_ptree: FORCE
	@echo -e "$(TESTCASE_COLOR)building ptree executable...$(NO_COLOR)"
	cd ptree && ndk-build

build_exec_test: FORCE
	@echo -e "$(TESTCASE_COLOR)building exec test executable...$(NO_COLOR)"
	cd exec_test && ndk-build

build_burger_buddies: FORCE
	@echo -e "$(TESTCASE_COLOR)building burger buddies executable...$(NO_COLOR)"
	cd burger_buddies && ndk-build

build_bbc_test:
	@echo -e "$(TESTCASE_COLOR)building burger buddies test...$(NO_COLOR)"
	cd burger_buddies_test && make

push: build_ptree_test build_ptree build_exec_test build_burger_buddies
	@echo -e "$(TESTCASE_COLOR)pushing executables...$(NO_COLOR)"
	@adb push ./ptree_test/obj/local/$(ARCH)/ptree_test $(INSTALL_PATH)
	@adb push ./ptree/obj/local/$(ARCH)/ptree $(INSTALL_PATH)
	@adb push ./exec_test/obj/local/$(ARCH)/exec_test $(INSTALL_PATH)
	@adb push ./burger_buddies/obj/local/$(ARCH)/BBC $(INSTALL_PATH)

rmmod: FORCE
	@echo -e "$(TESTCASE_COLOR)removing remote kernel module...$(NO_COLOR)"
	@adb shell "cd /data/local && rmmod ptree_module.ko"

install: build_ptree_module rmmod
	@echo -e "$(TESTCASE_COLOR)pushing kernel module...$(NO_COLOR)"
	@adb push ./ptree_module/ptree_module.ko $(INSTALL_PATH)
	@echo -e "$(TESTCASE_COLOR)installing kernel module...$(NO_COLOR)"
	@adb shell "cd $(INSTALL_PATH) && insmod ptree_module.ko"

test_ptree: install push
	@echo -e "$(TESTCASE_COLOR)running ptree tests...$(NO_COLOR)"
	@adb shell "cd $(INSTALL_PATH) && ./ptree_test"

run: install push
	@echo -e "$(TESTCASE_COLOR)running exec test...$(NO_COLOR)"
	@adb shell "cd $(INSTALL_PATH) && ./exec_test"

run_bbc: push
	@echo -e "$(TESTCASE_COLOR)running burger buddies simulation...$(NO_COLOR)"
	@adb shell "cd $(INSTALL_PATH) && ./BBC $(BBC_PARAMETER)" | tee ./burger_buddies/result/log.txt

test_bbc: push build_bbc_test
	@# cook cashier customer rack
	@echo -e "$(TESTCASE_COLOR)example in slides: 2 4 41 10$(NO_COLOR)"
	@adb shell "cd $(INSTALL_PATH) && ./BBC 2 4 41 10" > ./burger_buddies/result/log.txt
	@./burger_buddies_test/test ./burger_buddies/result/log.txt 2>/dev/null
	@echo -e "$(TESTCASE_COLOR)only one rack: 100 100 100 1$(NO_COLOR)"
	@adb shell "cd $(INSTALL_PATH) && ./BBC 100 100 100 1" > ./burger_buddies/result/log.txt
	@./burger_buddies_test/test ./burger_buddies/result/log.txt 2>/dev/null
	@echo -e "$(TESTCASE_COLOR)only one customer: 100 100 1 100$(NO_COLOR)"
	@adb shell "cd $(INSTALL_PATH) && ./BBC 100 100 1 100" > ./burger_buddies/result/log.txt
	@./burger_buddies_test/test ./burger_buddies/result/log.txt 2>/dev/null
	@echo -e "$(TESTCASE_COLOR)only one cashier: 100 1 100 100$(NO_COLOR)"
	@adb shell "cd $(INSTALL_PATH) && ./BBC 100 1 100 100" > ./burger_buddies/result/log.txt
	@./burger_buddies_test/test ./burger_buddies/result/log.txt 2>/dev/null
	@echo -e "$(TESTCASE_COLOR)only one cook: 1 100 100 100$(NO_COLOR)"
	@adb shell "cd $(INSTALL_PATH) && ./BBC 1 100 100 100" > ./burger_buddies/result/log.txt
	@./burger_buddies_test/test ./burger_buddies/result/log.txt 2>/dev/null
	@echo -e "$(TESTCASE_COLOR)few racks: 100 100 100 5$(NO_COLOR)"
	@adb shell "cd $(INSTALL_PATH) && ./BBC 100 100 100 5" > ./burger_buddies/result/log.txt
	@./burger_buddies_test/test ./burger_buddies/result/log.txt 2>/dev/null
	@echo -e "$(TESTCASE_COLOR)few customers: 100 100 5 100$(NO_COLOR)"
	@adb shell "cd $(INSTALL_PATH) && ./BBC 100 100 5 100" > ./burger_buddies/result/log.txt
	@./burger_buddies_test/test ./burger_buddies/result/log.txt 2>/dev/null
	@echo -e "$(TESTCASE_COLOR)few cashiers: 100 5 100 100$(NO_COLOR)"
	@adb shell "cd $(INSTALL_PATH) && ./BBC 100 5 100 100" > ./burger_buddies/result/log.txt
	@./burger_buddies_test/test ./burger_buddies/result/log.txt 2>/dev/null
	@echo -e "$(TESTCASE_COLOR)few cooks: 5 100 100 100$(NO_COLOR)"
	@adb shell "cd $(INSTALL_PATH) && ./BBC 5 100 100 100" > ./burger_buddies/result/log.txt
	@./burger_buddies_test/test ./burger_buddies/result/log.txt 2>/dev/null
	@echo -e "$(TESTCASE_COLOR)big data: 100 100 100 100$(NO_COLOR)"
	@adb shell "cd $(INSTALL_PATH) && ./BBC 100 100 100 100" > ./burger_buddies/result/log.txt
	@./burger_buddies_test/test ./burger_buddies/result/log.txt 2>/dev/null
	@echo -e "$(TESTCASE_COLOR)test sequence complete$(NO_COLOR)"

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
