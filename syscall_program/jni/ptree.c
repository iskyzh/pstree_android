#include <stdio.h>
#include "../../common/prinfo.h"

#define MAX_PRINFO 100
#define MAX_DEPTH 256

int main() {
    struct prinfo result[MAX_PRINFO];
    int n = MAX_PRINFO;
    int pid_stack[MAX_DEPTH];
    int i, j, depth = 0;
    syscall(383, &result, &n);
    printf("%d tasks read.\n", n);
    for (i = 0; i < n; i++) {
        while (depth > 0 && pid_stack[depth - 1] != result[i].parent_pid) depth--;
        for (j = 0; j < depth; j++) if (j != depth - 1) printf("    "); else printf("  |-");
        printf("%s\n", result[i].comm);
        pid_stack[depth++] = result[i].pid;
    }
    printf("OK.\n");
    return 0;
}
