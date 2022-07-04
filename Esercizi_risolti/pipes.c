#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

#define READ 0
#define WRITE 1
#define childrenN 3

pthread_t threads[childrenN];
int indeces[childrenN];
int fd[3][2];
void* listen();

int main(){
    
    pid_t root = getpid();
    printf("Root PID[%d]\n", root);
    for(int i=0; i<childrenN; ++i){
        pipe(fd[i]);
        if (getpid() == root){
            int id = fork();
            if (id < 0){
                perror("Forking error");
                exit(3);
            }
            if (!id){
                if (close(fd[i][0]) < 0){
                    perror("Child closing fd");
                    exit(2);
                }
                char pid[20];
                sprintf(pid, "%d", getpid());
                strcat(pid, " PID MSG");
                if (write(fd[i][1], pid, sizeof(pid)) == -1){
                    perror("Writing error");
                    exit(3);
                }
                exit(0);
            }
            indeces[i] = i;
        }

    }
    
    if (getpid() == root){
        char buf[256];

        for(int i=0; i<childrenN; ++i){
            if (pthread_create(&threads[i], NULL, &listen, (void*) &indeces[i]) != 0){
                perror("Thread error");
                exit(1);
            }
            printf("Created thread %d\n", i);
        }

        for(int i=0; i<childrenN; ++i){
            if (pthread_join(threads[i], NULL) != 0){
                perror("Thread join error");
                exit(4);
            }
        }
    }
    while(wait(NULL)>0);

    return 0;
}

void* listen(void* ind){
    int index = *(int*) ind;
    char buffer[256];
    if (close(fd[index][1]) < 0){
        perror("Root closing fd");
        exit(1);
    }
    if (read(fd[index][0], buffer, sizeof(buffer)) < 0){
        printf("Index %d\n", index); fflush(stdout);
        perror("Reading error");
        exit(6);
    }
    printf("%s\n", buffer);
    fflush(stdout);
    close(fd[index][0]);

    return (void*)0;
}