#include <stdio.h>
#include "../../common/prinfo.h";

#define MAX_PRINFO 100

int main() {
    struct prinfo result[MAX_PRINFO];
    int n = MAX_PRINFO;
    syscall(100, &result, &n);
    printf("%d tasks read.\n", n);
    for (int i = 0; i < n; i++) {
        printf("%s %d %d %d %d %d %d\n", result[i].comm,
            result[i].parent_pid,
            result[i].pid,
            result[i].first_child_pid,
            result[i].next_sibling_pid,
            result[i].state,
            result[i].uid);
    }
    printf("OK.\n");
    return 0;
}
