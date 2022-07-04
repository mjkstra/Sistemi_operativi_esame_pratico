#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>

#define MAX_CHILDREN 5
#define MAX_INPUT 30
int childNum = 0;
char string[MAX_INPUT];
pid_t children[MAX_CHILDREN] = {0, 0, 0, 0, 0};
pid_t root;
pid_t localPID;

int launch_thread(int** pipes);
void handler(int sigNum);
int pipes[MAX_CHILDREN][2];
void* sendMessages(int** pipes);

int main(){
    printf("Root pid [%d]\n", root = getpid());
    int num = -1;
    signal(SIGUSR1, handler);
    signal(SIGUSR2, handler);
    signal(SIGINT, handler);
    for (int i=0; i<MAX_CHILDREN; ++i){
        pipe(pipes[i]);
    }
    while (1){
        if (root == getpid()){
            char buffer[256];
            fgets(buffer, sizeof(buffer), stdin);
            num = atoi(buffer);
            if (num == 0){
                printf("%s\n", string);
                strcpy(string, buffer);
            } else {
                num = num % MAX_CHILDREN;
                printf("%d\n", num);
                if (childNum < MAX_CHILDREN){
                    childNum++;
                    int id = fork();
                    if (!id){ // child
                        for(int i=0; i<MAX_CHILDREN; ++i){
                            if (i != num){
                                close(pipes[i][0]);
                                close(pipes[i][1]);
                            }
                        }
                        children[num] = getpid();
                        close(pipes[num][1]);
                        // get ready to read
                        pid_t pid;
                        char buffer[MAX_INPUT];
                        read(pipes[num][0], &pid, sizeof(pid_t));
                        read(pipes[num][0], buffer, sizeof(buffer));
                        printf("[CHILD %d]: Received message: '%s'\n", pid, buffer);
                    } else {
                        children[num] = id;
                        for (int i=0; i<MAX_CHILDREN; ++i){
                            close(pipes[i][0]);
                        }
                    }
                } else {
                    printf("You already have 5 kids more than what you need!\n");
                }
            }
        }
    }
    while(wait(NULL)>0);
    return 0;
}

void handler(int sigNum){
    printf("Received signal %d\n", sigNum);
    if (sigNum == SIGUSR1 || sigNum == SIGUSR2){
        launch_thread(pipes);
    } else if (sigNum == SIGINT){
        printf("Killing root and children processes\n");
        for(int i=0; i<MAX_CHILDREN; ++i){
            kill(children[i], SIGINT);
        }
        exit(0);
    }
}

int launch_thread(int** pipes){
    printf("Launching thread...\n");
    pthread_t id;
    pthread_create(&id, NULL, sendMessages, (void* )&pipes);
}

void* sendMessages(int** pipes){
    printf("Printing messages...\n");
    for(int i=0; i<MAX_CHILDREN; ++i){
        printf("Children[%d]>> PID: %d\n", i, children[i]);
        if (children[i] != 0){
            printf("Printing %d and '%s'\n", children[i], string);
            if (write(pipes[i][1], i, sizeof(int))< 0){
                printf("Something went wrong");
                exit(1);
            } 
            if (write(pipes[i][1], getpid(), sizeof(pid_t)) < 0){
                printf("DIOBOIA1\n");
                exit(1);
            } 
            if (write(pipes[i][1], string, sizeof(string)) < 0){
                printf("DIOCANE1\n");
                exit(1);
            } 
            children[i] = 0;
        }
        printf("Printed!\n");
    }
    printf("Finished printing\n");
    return (void *)NULL;
}