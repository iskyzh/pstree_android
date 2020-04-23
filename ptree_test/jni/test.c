/** 
 * Test cases for ptree syscall
 * 
 * This program tests ptree syscall against invalid input,
 * concurrent call and small buffer. You may run this program
 * by running `make test_ptree`, that will automatically build,
 * push and run this program on Android emulator.
*/

#include <stdio.h>
#include <pthread.h> 
#include "../../common/prinfo.h"

#define MAX_PRINFO 100
#define KERNEL_ADDR 0xc000d8c4

/// ptree syscall
int ptree(struct prinfo* result, int* n) {
    return syscall(356, result, n);
}

void test_suite(const char* case_name) {
    printf("%s\n", case_name);
}

void test_case(const char* case_name) {
    printf("   %s... ", case_name);
}

/// print OK when x != 0, and fail when x = 0
void test_assert(int x) {
    if (x) printf("\033[32mOK");
    else printf("\033[31mFailed");
    printf("\033[39m\n");
}

/// test pointer of NULL or to kernel space
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

/// test if ptree works well on small buffer
int test_functionality() {
    struct prinfo result[MAX_PRINFO];
    int n = MAX_PRINFO;
    int ret = 0;
    test_suite("functionality test");
    test_case("normal execution");
        ret = ptree(result, &n);
        test_assert(ret >= 0 && n > 0);
    test_case("very small n");
        n = 10;
        ret = ptree(result, &n);
        test_assert(ret > 10 && n == 10);
    test_case("zero n");
        n = 0;
        ret = ptree(result, &n);
        test_assert(ret < 0 && n == 0);
    test_case("negative n");
        n = -1;
        ret = ptree(result, &n);
        test_assert(ret < 0 && n == -1);
    return 0;
}

/// test some cases that might cause page fault
int test_edge_case() {
    struct prinfo *result = malloc(1000 * sizeof(struct prinfo));
    int n = 1000;
    int ret = 0;
    test_suite("edge case test");
    test_case("large prinfo array");
        ret = ptree(result, &n);
        test_assert(ret >= 0 && n > 0);
    free(result);
    return 0;
}

/// concurrent test thread
void* concurrent_test(void* ret_val) {
    struct prinfo *result = malloc(100 * sizeof(struct prinfo));
    int n = 100;
    int *ret = ret_val;
    *ret = ptree(result, &n);
    free(result);
    return NULL;
}

/// concurrent test thread that force ptree to fail
void* concurrent_test_fail(void* ret_val) {
    int n = 100;
    int *ret = ret_val;
    ptree(NULL, &n);
    *ret = 1;
    return NULL;
}

#define MAX_CONCURRENT 50

/// test concurrent requests to kernel ptree module
int test_concurrent() {
    pthread_t tid[MAX_CONCURRENT];
    int result[MAX_CONCURRENT] = { 0 };
    int i;

    test_suite("concurrent test");
    {
        test_case("spawn 50 ptree");

        for (i = 0; i < MAX_CONCURRENT; i++) {
            pthread_create(&tid[i], NULL, concurrent_test, &result[i]);
        }

        for (i = 0; i < MAX_CONCURRENT; i++) {
            pthread_join(tid[i], NULL);
        }

        int ok = 1;
        for (i = 0; i < MAX_CONCURRENT; i++) {
            if (result[i] <= 0) ok = 0;
        }

        test_assert(ok);
    }

    {
        test_case("spawn 50 ptree with wrong parameter");

        for (i = 0; i < MAX_CONCURRENT; i++) {
            pthread_create(&tid[i], NULL, concurrent_test_fail, &result[i]);
        }

        for (i = 0; i < MAX_CONCURRENT; i++) {
            pthread_join(tid[i], NULL);
        }

        int ok = 1;
        for (i = 0; i < MAX_CONCURRENT; i++) {
            if (result[i] <= 0) ok = 0;
        }
        
        test_assert(ok);
    }

    return 0;
}

int main() {
    test_invalid_pointer();
    test_functionality();
    test_edge_case();
    test_concurrent();
    printf("Test sequence complete.\n");
    return 0;
}
