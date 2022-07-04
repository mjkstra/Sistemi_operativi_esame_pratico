#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(){
    pid_t root = getpid();
    for(int i=0; i<3; ++i){
        if (root == getpid()) fork();
    }
    printf("Hello world\n");

    return 0;
}