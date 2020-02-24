#ifndef ALEX_MODULE_PRINFO_HEADER
#define ALEX_MODULE_PRINFO_HEADER

struct prinfo {
    pid_t parent_pid;
    pid_t pid;
    pid_t first_child_pid;
    pid_t next_sibling_pid;
    long state;
    long uid;
    char comm[64];
};

#endif
