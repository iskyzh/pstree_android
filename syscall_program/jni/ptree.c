#include <stdio.h>
#include "../../common/prinfo.h"

#define MAX_PRINFO 100
#define MAX_DEPTH 256

int ptree(struct prinfo* result, int* n) {
    return syscall(383, result, n);
}

int main() {
    struct prinfo result[MAX_PRINFO];
    int n = MAX_PRINFO;
    int pid_stack[MAX_DEPTH];
    int i, j, depth = 0;
    ptree(&result, &n);
    printf("%d tasks read.\n", n);
    for (i = 0; i < n; i++) {
        struct prinfo* p = &result[i];
        while (depth > 0 && pid_stack[depth - 1] != p->parent_pid) depth--;
        for (j = 0; j < depth; j++) if (j != depth - 1) printf("    "); else printf("  |-");
        printf("%s,%d,%ld,%d,%d,%d,%d\n",
            p->comm, p->pid, p->state,
            p->parent_pid, p->first_child_pid,
            p->next_sibling_pid, p->uid);
        pid_stack[depth++] = p->pid;
    }
    printf("OK.\n");
    return 0;
}
