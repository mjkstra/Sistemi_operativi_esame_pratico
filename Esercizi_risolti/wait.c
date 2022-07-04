#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <stdio.h>

int main(){
    int id = fork();

    int n;

    if (id == 0){
        n = 1;
    } else {
        wait(NULL);
        n = 6;
    }
    int i;
    for (i=n ; i<n+5; ++i){
        printf("%d ", i); 
        //fflush(stdout);
    }
    if (id)
        printf("\n");
    return 0;
}