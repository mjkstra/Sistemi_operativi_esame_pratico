#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

int main(){
    int id = fork();

    if (id == 0){
        sleep(1);
    }
    printf("Current ID:[%d]\tParent ID:[%d]\n", getpid(), getppid());
    int res = wait(NULL);
    if (res == 1){
        printf("No children to wait for\n");
    } else {
        printf("%d finished execution\n", res);
    }

    return 0;
}