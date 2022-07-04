#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>

#define BUF_SIZE 30
#define MAX_CHILDREN 5
int n;
int cCount = 0;
pid_t root;
pid_t children[MAX_CHILDREN];
void handler(int sigNum, siginfo_t* info, void* context);
int pipes[MAX_CHILDREN][2];
FILE* stream;

int lastSig;

struct msg {
    int index;
    int id;
    char msg[256];
} msg1, msg2;

char buffer[BUF_SIZE];
char lastString[BUF_SIZE];
int main(int argc, char** argv){
    printf("Root %d\n", root = getpid());
    for(int i=0; i<MAX_CHILDREN; ++i){
        children[i] = 0;
    }
    stream = fopen("/tmp/log.txt", "w+");
    
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO | SA_RESTART;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = &handler;  
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);

    do {
        scanf("%s", buffer);
        n = atoi(buffer);
        cCount = 0;
        for(int i=0; i<MAX_CHILDREN; ++i){
            if (children[i]!=0) cCount++;
        }
        if (n == 0){
            // string received
            strcpy(lastString, buffer);
        } else {
            // number received
            if (cCount < MAX_CHILDREN){
                if (pipe(pipes[cCount++]) < 0){
                    perror("Pipe opening error");
                    exit(3);
                }
                printf("cCount before forking %d\n", cCount);
                int id = fork();
                if (id == -1){
                    perror("Forking error");
                    exit(1);
                }
                if (id){
                    children[cCount-1] = id;
                    // pipes[cCount-1][1]; // write later
                    if (close(pipes[cCount-1][0]) < 0){
                        printf("Piping error with child pipe[%d][%d]\n", cCount-1 , 0);
                        perror("");
                        exit(2);
                    }
                } else {
                    int i;
                    for(i=0; i<cCount; ++i){
                        printf("Chidren Count %d\n", cCount);
                        if (close(pipes[i][1]) < 0){
                            printf("Piping error with child pipe[%d][%d]\n", i , 1);
                            perror("");
                            exit(2);
                        }
                    }
                    read(pipes[i-1][0], &msg2, sizeof(msg2));
                    printf("Id[%d] of index[%d] with message: '%s'\n", msg2.id, msg2.index, msg2.msg);
                    fflush(stdout);
                    close(pipes[i-1][0]);
                    printf("Closed reading end of child[%d]\n", i);
                }
            } else {
                printf("Don't you have enough kids?!\n");
            }
        }
    } while (getppid() == root || getpid() == root);
    fclose(stream);

    return 0;
}

void* sendMSG(void* ind){
    int index = *(int*) ind;
    msg1.id = children[index];
    msg1.index = index;
    strcpy(msg1.msg, lastString);
    write(pipes[index][1], &msg1, sizeof(msg1));
    // document on log file
    fprintf(stream, "Received signal %d\n", lastSig);
    fflush(stream);
    cCount = 0;
    return (void *)0;
}

void handler(int sigNum, siginfo_t* info, void* context){
    ////
    if (sigNum == SIGUSR1 || sigNum == SIGUSR2){
        if (getpid() == root){
            for(int i=0; i<cCount; ++i){
                lastSig = info->si_signo;
                pthread_t t1;
                if (pthread_create(&t1, NULL, &sendMSG, (void *) &i) != 0){
                    perror("Thread creation error");
                    exit(5);
                }

                if (pthread_join(t1, NULL) != 0){
                    perror("Thread waiting error");
                    exit(6);
                }

                children[i] = 0;
                if (close(pipes[i][1]) < 0){
                    perror("Closing pipe after writing error");
                    exit(3);
                }
                pthread_cancel(t1);
            }
            cCount = 0;
        }
    } else if (sigNum == SIGINT){
        for(int i=0; i<cCount; ++i){
            kill(children[i], SIGTERM);
        }
        fclose(stream);
        exit(0);
    }
}
