#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

pid_t root;
pid_t manager;
pid_t children[10];
int n;

int pipes[10][2];

void checkFile(const char*);
void handler(int sigNum, siginfo_t* info, void* context);

int main(int argc, char** argv){

    if (argc != 3){
        perror("Inappropriate usage!\n");
        exit(3);
    }
    n = atoi(argv[2]);
    if (n < 0 || n > 10){
        perror("Invalid number! Must be in range [1,10]\n");
        exit(4);
    }
    checkFile(argv[1]);
    printf("Checked file\n");

    // signals
    struct sigaction managerS;
    managerS.sa_sigaction = &handler;
    sigemptyset(&managerS.sa_mask);
    managerS.sa_flags = SA_SIGINFO;
    sigaddset(&managerS.sa_mask, SIGALRM);
    sigaction(SIGUSR1, &managerS, NULL);
    sigaction(SIGTERM, &managerS, NULL);

    struct sigaction childrenS;
    childrenS.sa_sigaction = &handler;
    sigemptyset(&childrenS.sa_mask);
    managerS.sa_flags = SA_SIGINFO;
    sigaddset(&childrenS.sa_mask, SIGCHLD);
    sigaddset(&childrenS.sa_mask, SIGCONT);
    sigaction(SIGUSR1, &childrenS, NULL);
    sigaction(SIGTERM, &childrenS, NULL);
    
    printf("Defined handlers\n");
    // pipes
    // for(int i=0; i<n; ++i){
    //     pipe(pipes[i]);
    // }
    for(int i=0; i<n; ++i){
        pipe(pipes[i]);
    }
    printf("Pipes created\n");
    //forking
    FILE* file = fopen(argv[1], "w+");
    fprintf(file, "Root[%d]\n", root = getpid()); fflush(file);
    printf("Opened file\n");
    int id = fork();
    if (id == -1){
        perror("Forking error\n");
        exit(6);
    }
    if (!id){ //child
        fprintf(file, "Manager[%d]\n", manager = getpid()); fflush(file);
        for (int i=0; i<n; ++i){
            if (getpid() == manager){
                int child = !fork();
                if (child){
                    fprintf(file, "Child[%d]\n", children[i] = getpid()); fflush(file);
                    for(int j=0; j<n; ++j){  
                        if (close(pipes[j][1])<0) {
                            perror("Pipe writing-end error\n");
                            printf("It j[%d] i[%d]\n", j, i);
                            exit(7);
                        };
                        if (i!=j){
                            if (close(pipes[j][0])<0) {
                                perror("Pipe writing-end error\n");
                                printf("It j[%d] i[%d]\n", j, i);
                                exit(7);
                            };
                        }                 
                    }
                    fclose(file);
                    pid_t externalProcess;
                    printf("Process[%d][%d] waiting to read...\n", i, children[i]);
                    //close(pipes[0][0]);
                    if (read(pipes[i][0], &externalProcess, sizeof(pid_t)) < 0){
                        perror("Reading error: ");
                        printf("CHLD[%d]\n", i);
                        exit(8);
                    }
                    printf("[CHLD[%d]] Killing process[%d]\n", i, externalProcess);
                    //close(pipes[i][0]);
                    kill(externalProcess, SIGUSR2);
                    exit(0);
                } else {
                    
                }
            }
        }
        if (getpid() == manager){
            for(int i=0; i<n; ++i){
                if (close(pipes[i][0])<0) {
                    perror("Error closing reading end");
                    exit(10);
                }
            }
        }
        while(wait(NULL)>0);
    } 
    fclose(file);
    return 0;
}

void checkFile(const char* path){
    int fd = open(path, O_RDONLY, 777);
    if (fd > 0){
        perror("File already exists!\n");
        exit(5);
        if (close(fd)<0){
            perror("Error with file closing");
            exit(5);
        }
    }
}

void handler(int sigNum, siginfo_t* info, void* context){
    if (sigNum == SIGUSR1){
        if (getpid() == manager){
            if (write(pipes[n-1][1], &info->si_pid, sizeof(pid_t)) <= 0){
                perror("Errore in scrittura SIGUSR1\n");
            }
            children[n-1] = 0; n--;
        } else {
            printf("Unexpected behaviour\n");
        }
    } else if (sigNum == SIGTERM){
        for(int i=0; i<n; ++i){
            kill(children[i], SIGINT);
        }
        kill(manager, SIGINT);
    } else {
        if (getpid() == manager && sigNum == SIGALRM){
            perror("Shouldn't receive this shit\n");
        }
        if (getppid() == manager && (sigNum == SIGCHLD || sigNum == SIGCONT)){
            perror("Shouldn't receive this shit");
        }
    }
}