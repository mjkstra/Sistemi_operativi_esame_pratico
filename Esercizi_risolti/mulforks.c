#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/wait.h>
#include <errno.h>

int main(){
    int id1 = fork();
    int id2 = fork();

    printf("Hello Word from %d!\n", getpid());
    if (id1 == 0){
        if (id2 == 0){
            printf("I'm process y\n");
        } else {
            printf("I'm process x\n");
        }
    } else {
        if (id2 == 0){
            printf("I'm process z\n");
        } else {
            printf("I'm the parent process\n");
        }
    }

    while (wait(NULL) != -1 || errno != ECHILD){
        printf("Waited for a child to finish\n");
    }
    return 0;
}