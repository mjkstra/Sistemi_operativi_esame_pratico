#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#define MAX_SENDER 5
typedef enum { false, true } bool;

struct process {
    pid_t id;
    int sig1;
    int sig2;
};


pid_t root;
struct process* processes[MAX_SENDER];
int currentProcesses = 0;
int handler(int sigNum, siginfo_t info, void* context);
bool foreground();

int main(int argc, char** argv){
    printf("Root PID %d\n", root = getpid());
    signal(SIGUSR1, handler);
    signal(SIGUSR2, handler);
    signal(SIGINT, handler);
    signal(SIGTERM, handler);
    for(int i=0; i<MAX_SENDER; ++i){
        if (getpid() == root){
            int id = fork();
        }
    }
    
    while(1);
    return 0;
}

int handler(int sigNum, siginfo_t info, void* context){
    
    if (sigNum == SIGUSR1 || sigNum == SIGUSR2){
        pid_t incoming = info.si_pid;
        int index;
        for(int i=0; i<MAX_SENDER; ++i){
            printf("Diocane\n");
            if (processes[i] == NULL)
                index = i;
            else if ((*processes[i]).id == incoming){
                index = -1;
            }
        }

        if (index != -1){
            struct process* tmp = malloc(sizeof(struct process));
            tmp->id = incoming;
            tmp->sig1 = 0; tmp->sig2 = 0;
            processes[index] = tmp;
        }
        printf("Created new processes\n");
        if (foreground()){
            printf("Signal code %d, Sender %p\n", sigNum, info.si_addr);
        }
        for(int i=0; i<MAX_SENDER; ++i){
            if (processes[i] != NULL){                
                if (sigNum == SIGUSR1)
                    processes[i]->sig1++;
                else
                    processes[i]->sig2++;
            }
        }
    } else if (sigNum == SIGINT || sigNum == SIGTERM){
        for(int i=0; i<MAX_SENDER; ++i){
            if (processes[i] != NULL){
                printf("PID[%d]\tSIGUSR1[%d]\tSIGUSR2[%d]\n", processes[i]->id, processes[i]->sig1, processes[i]->sig2);
            }
        }
        exit(1);
    }
}

bool foreground() {
  //  tcgetpgrp(fd):
  //    process group ID of foreground process group for terminal of “fd”
  //    (should be the controlling terminal of the calling process)
  //  getpgrp():
  //    returns process group ID of the calling process
  bool fg=false;
  if (getpgrp() == tcgetpgrp(STDOUT_FILENO)) fg=true;
  return fg;
}