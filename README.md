# OS Project 1

I've made a Makefile for this project. It should be very easy to use if you have all required tools installed :)

## TL;DR

First of all, configure environment varables required for this project. Just specify kernel directory with $KID.

```bash
export KID=$HOME/kernel/goldfish
```

Then, start the emulator. Wait until `adb devices` can list the emulator device.

After all, just invoke these four commands to test my solution.

```bash
make all
make test_bbc
make run_bbc BBC_PARAMETER="2 4 41 10"
make rmmod
```

`make all` tests ptree kernel module automatically with my own test case, and prints a process tree in problem 3.

`make test_bbc` automatically tests my burger buddies solution, by first redirecting BBC log on Android device to local `log.txt`, and then use my test script to check if there's any inconsistency in the log file.

`make run_bbc` will print out logs for Burger Buddies problem.

`make rmmod` removes my kernel module (but not delete the file). Run this command and then you can test next student's project.

That's all :)

## Project Structure

```
.
├── Makefile                    Makefile for whole project
├── README.md                   this file
├── burger_buddies              Burger Buddies solution
│   ├── jni
│   │   ├── Android.mk
│   │   └── BurgerBuddies.c
│   └── result                  Burger Buddies log output folder
│       └── dummy.txt
├── burger_buddies_cond         A Burger Buddies solution based on condition variable
│   └── main.cpp
├── burger_buddies_test         Burger Buddies log checker
│   ├── Makefile
│   └── test.cpp
├── common                      prinfo structure used in both kernel and user space
│   └── prinfo.h
├── exec_test                   problem 3, spawn two process (parent/child)                
│   └── jni
│       ├── Android.mk
│       └── exec_test.c
├── ptree                       problem 2, ptree program
│   └── jni
│       ├── Android.mk
│       └── ptree.c
├── ptree_module                problem 1, ptree kernel module
│   ├── Makefile
│   ├── gen.h
│   ├── gen.sh
│   └── ptree_module.c
└── ptree_test                  test cases for ptree kernel module
    └── jni
        ├── Android.mk
        └── test.c
```

## More Instructions

If you want to run the building, pushing and testing procedures manually, here I provide a list of all functions in the Makefile.

Run my own test for ptree.
```bash
make test_ptree
```

Run problem 3, test ptree by spawning parent and child process.
```bash
make run
```

Just run `ptree` program.
```bash
make run_ptree
```

Run ptree test and problem 3.
```bash
make all
```

Run my tests for burger buddies program.
```bash
make test_bbc
```

Run Burger Buddies on Android emulator with parameters.
```bash
make run_bbc BBC_PARAMETER="2 4 41 10"
```

Build and push all executables to Android emulator `/data/local`.
```bash
make push
```

Build and push kernel module.
```bash
make install
```

Remove kernel module.
```bash
make rmmod
```

Clean workspace.
```bash
make clean
```

Thanks for reading this long readme! In project report I highlighted some special works I've done in this project. While waiting for building and testing, you may take a glance of that.
