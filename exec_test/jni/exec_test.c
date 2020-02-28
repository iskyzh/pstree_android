#include <stdio.h>
#include <unistd.h>

int main() 
{
    if (fork() == 0) {
        printf("StudentIDChild is %d\n", getpid());
        char *args[]={ "./ptree", NULL };
        execvp(args[0], args);     
    } else {
        printf("StudentIDParent is %d\n", getpid());
        wait(NULL);
    }
    return 0; 
}
