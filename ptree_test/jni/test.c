#include <stdio.h>
#include "../../common/prinfo.h"

#define MAX_PRINFO 100
#define KERNEL_ADDR 0xc000d8c4

int ptree(struct prinfo* result, int* n) {
    return syscall(383, result, n);
}

void test_suite(const char* case_name) {
    printf("%s\n", case_name);
}

void test_case(const char* case_name) {
    printf("   %s... ", case_name);
}

void test_assert(int x) {
    if (x) printf("\033[32mOK");
    else printf("\033[31mFailed");
    printf("\033[39m\n");
}

int test_invalid_pointer() {
    struct prinfo result[MAX_PRINFO];
    int n = MAX_PRINFO;
    int ret = 0;
    test_suite("invalid pointers");
    test_case("kernel ptr in buf");
        ret = ptree((struct prinfo*) KERNEL_ADDR, &n);
        test_assert(ret < 0);
    test_case("kernel ptr in nr");
        ret = ptree(result, (int*) KERNEL_ADDR);
        test_assert(ret < 0);
    test_case("kernel ptr in both fields");
        ret = ptree((struct prinfo*) KERNEL_ADDR, (int*) KERNEL_ADDR);
        test_assert(ret < 0);
    test_case("NULL pointer in buf");
        ret = ptree(NULL, &n);
        test_assert(ret < 0);
    test_case("NULL ptr in nr");
        ret = ptree(result, NULL);
        test_assert(ret < 0);
    test_case("NULL ptr in both fields");
        ret = ptree(NULL, NULL);
        test_assert(ret < 0);
    return 0;
}

int test_functionality() {
    struct prinfo result[MAX_PRINFO];
    int n = MAX_PRINFO;
    int ret = 0;
    test_suite("functionality test");
    test_case("normal execution");
        ret = ptree(result, &n);
        test_assert(ret == 0 && n > 0);
    test_case("very small n");
        n = 10;
        ret = ptree(result, &n);
        test_assert(ret == 0 && n == 10);
    test_case("zero n");
        n = 0;
        ret = ptree(result, &n);
        test_assert(ret == 0 && n == 0);
    return 0;
}

int main() {
    test_invalid_pointer();
    test_functionality();
    printf("Test sequence complete.\n");
    return 0;
}
