#include <stdio.h>
#include <stdlib.h>
#include "../../common/prinfo.h"

#define MAX_PRINFO 100
#define MAX_DEPTH 256

int ptree(struct prinfo* result, int* n) {
    return syscall(383, result, n);
}

int main(int argc, char *argv[]) {
    int n = MAX_PRINFO;
    int pid_stack[MAX_DEPTH];
    struct prinfo result[MAX_PRINFO];
    int i, j, depth = 0;
    int ret = ptree(result, &n);
    int parent_pid = -1, child_pid = -1;
    if (ret < 0) {
        printf("error while ptree: %d.\n", ret);
        return -1;
    }
    if (ret == MAX_PRINFO) {
        printf("task number may have exceeded limit %d.\n", MAX_PRINFO);
    }
    if (argc == 3) {
        parent_pid = atoi(argv[1]);
        child_pid = atoi(argv[2]);
    }
    printf("%d tasks read.\n", n);
    for (i = 0; i < n; i++) {
        struct prinfo* p = &result[i];
        while (depth > 0 && pid_stack[depth - 1] != p->parent_pid) depth--;
        for (j = 0; j < depth; j++) if (j != depth - 1) printf("    "); else printf("  |-");
        if (p->pid == parent_pid || p->pid == child_pid) printf("\033[32m");
        printf("%s,%ld,%ld,%d,%d,%d,%d\n",
            p->comm, p->pid, p->state,
            p->parent_pid, p->first_child_pid,
            p->next_sibling_pid, p->uid);
        if (p->pid == parent_pid || p->pid == child_pid) printf("\033[39m");
        pid_stack[depth++] = p->pid;
    }
    printf("OK.\n");
    return 0;
}
