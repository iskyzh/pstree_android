#include <stdio.h>

int main() {
    syscall(100, 1, 1);
    printf("OK.\n");
    return 0;
}
