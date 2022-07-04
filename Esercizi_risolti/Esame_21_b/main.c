#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int n;
pid_t root;
pid_t* children;
int queueID;
int* fd;
struct msg_buffer{
    long mtype;
    char mtext[100];
} msgpSND, msgpRCV;

void handler(int sigNum, siginfo_t* info, void* context);

int main(int argc, char ** argv){
    root = getpid();
    if (argc != 3){
        fprintf(stderr, "Use %s <path> <n>\n", argv[0]); fflush(stderr);
        exit(1);
    }
    n = atoi(argv[2]);
    if (n < 1 || n > 10){
        fprintf(stderr, "N must be in range [1,10]\n"); fflush(stderr);
        exit(2);
    }
    children = malloc(n*sizeof(pid_t));
    fd = malloc(n*sizeof(int));
    if (chdir(argv[1]) == -1){ //fchdir turns the selected dir as cwd
        perror("Invalid directory:");
        exit(3);
    }
    
    if (mkdir("info", 0755) == -1){
        perror("Error while creating directory\n");
        exit(4);
    }
    if (chdir("info") == -1){
        perror("Errore while changing directory\n");
        exit(4);
    }
    int file = creat("key.txt", 0755);
    char buf[6];
    if (sprintf(buf, "%d\n", root) < 0){
        perror("Something happened with sprintf: ");
        exit(4);
    }
    printf("%s", buf);
    if (write(file, buf, sizeof(buf)) < 0){
        perror("Writing on file key.txt error: ");
        exit(5);
    }

    // queue creation
    system("pwd");
    key_t queueKey = ftok("key.txt", 32);
    queueID = msgget(queueKey, 0755 | IPC_CREAT);
    if (queueID == -1){
        perror("Queue creation error ");
        exit(10);
    }
    printf("QueueID: %d", queueID);
    if (queueID == 0){
        msgctl(queueID,  IPC_RMID, NULL);
        queueID = msgget(queueKey, IPC_CREAT | 0755);
    }
    sprintf(buf, "%d", root);
    strcpy(msgpSND.mtext, buf);
    msgpSND.mtype = 1;
    if (msgsnd(queueID, &msgpSND, sizeof(msgpSND.mtext), 0) < 0 ){
        perror("diocane");
        strerror(errno);
        exit(5);
    }
   
    printf("Madonna boia %s\n", msgpRCV.mtext);
    fflush(stdout);
    sleep(1);

    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO | SA_RESTART;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = &handler;
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);

    for(int i=0; i<n; ++i){
        if (getpid() == root){
            int id = fork();
            if (id){
                children[i] = id;
                char path[15];
                sprintf(path, "%d.txt", id);
                fd[i] = creat(path, 755);
            } else {
                
            }
        }
    }
    if (getpid() == root){
        for(int i=0; i<n-1; ++i){
            printf("%d ", children[i]);
        }
        printf("%d\n", children[n-1]);
    }
    while(1){
        sleep(1);
    }

    return 0;
}

void handler(int sigNum, siginfo_t* info, void* context){
    if (getpid() != root){
        if (sigNum == SIGUSR1){
            for(int i=0; i<n; ++i){
                if (getpid() == children[i]){
                    write(fd[i], "SIGUSR1\n", sizeof("SIGUSR1\n"));
                }
            }
        } else if (sigNum == SIGUSR2){
            for(int i=0; i<n; ++i){
                if (getpid() == children[i]){
                    char buf[10];
                    sprintf(buf, "%d", children[i]);
                    strcpy(msgpSND.mtext, buf);
                    if (msgsnd(queueID, &msgpSND, sizeof(msgpSND.mtext), 0) <= 0){
                        perror("Writing error\n");
                        exit(6);
                    }
                    printf("Printed msg\n");
                }
            }
        } else if (sigNum == SIGINT){
            kill(getppid(), SIGINT);
        }
    } else {
        if (sigNum == SIGINT){
            printf("OK\n");
            while(msgrcv(queueID, &msgpRCV, sizeof(msgpRCV.mtext), 0, 0) > 0){
                printf("%s\n", msgpRCV.mtext);
                fflush(stdout);
            }
        }
    }
}