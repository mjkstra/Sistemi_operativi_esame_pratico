#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

void* foo(int a){
    printf("%d\n", a++);

}

int a = 1;
int main(){
    pthread_t t1, t2;
    int b = 2;
    pthread_create(&t1, NULL, &foo, a);
    pthread_join(t1, NULL);
    pthread_create(&t2, NULL, &foo, a);
    pthread_join(t2, NULL);
    return 0;
}
