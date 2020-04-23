#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() 
{
    int parent_pid = getpid();
    int child_pid = 0;
    char arg1[100] = { 0 };
    char arg2[100] = { 0 };
    int ret = 0;

    if (fork() == 0) {
        child_pid = getpid();
        printf("518021910395 Child is %d\n", child_pid);
        sprintf(arg1, "%d", parent_pid);
        sprintf(arg2, "%d", child_pid);
        char *args[] = { "./ptree", arg1, arg2, NULL };
        if (ret = execvp(args[0], args) < 0) {
            printf("failed to execute ptree, retval = %d\n", ret);
            return -1;
        }
    } else {
        printf("518021910395 Parent is %d\n", getpid());
        return wait(NULL);
    }
}
