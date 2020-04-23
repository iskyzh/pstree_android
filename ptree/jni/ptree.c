/**
 * ptree program 
 * 
 * This program will call ptree module and print out
 * the process tree. You may build, push and run this
 * program with `make run_ptree`.
 * 
 * If this program is called by exec test, parent process
 * and child process will be highlighted in green color.
*/

#include <stdio.h>
#include <stdlib.h>
#include "../../common/prinfo.h"

#define MAX_PRINFO 100
#define MAX_DEPTH 256

/// ptree syscall
/// @param result: prinfo buffer
/// @param n: maximum number of elements can be held,
///           will be written to actual copied element number
/// @returns number of tasks in system
int ptree(struct prinfo* result, int* n) {
    return syscall(356, result, n);
}

int main(int argc, char *argv[]) {
    int n = MAX_PRINFO;
    int pid_stack[MAX_DEPTH];
    struct prinfo result[MAX_PRINFO];
    int i, j, depth = 0;
    int ret = ptree(result, &n);
    int parent_pid = -1, child_pid = -1;

    // exit if prinfo goes wrong
    if (ret <= 0) {
        printf("error while ptree: %d.\n", ret);
        return -1;
    }

    // warn if there's more task in system
    if (ret > MAX_PRINFO) {
        printf("task number may have exceeded limit %d.\n", MAX_PRINFO);
    }

    // get parent pid and child pid to highlight (problem 3)
    if (argc == 3) {
        parent_pid = atoi(argv[1]);
        child_pid = atoi(argv[2]);
    }
    printf("%d tasks read.\n", n);

    for (i = 0; i < n; i++) {
        struct prinfo* p = &result[i];
        // try to figure out who's parent of this task
        while (depth > 0 && pid_stack[depth - 1] != p->parent_pid) depth--;
        // print depth number of space before output
        for (j = 0; j < depth; j++) if (j != depth - 1) printf("    "); else printf("  |-");
        // if this task should be highlighted, output control sequence
        if (p->pid == parent_pid || p->pid == child_pid) printf("\033[32m");
        // print task info
        printf("%s,%d,%ld,%d,%d,%d,%ld\n",
            p->comm, p->pid, p->state,
            p->parent_pid, p->first_child_pid,
            p->next_sibling_pid, p->uid);
        if (p->pid == parent_pid || p->pid == child_pid) printf("\033[39m");
        pid_stack[depth++] = p->pid;
    }
    printf("OK.\n");
    return 0;
}
