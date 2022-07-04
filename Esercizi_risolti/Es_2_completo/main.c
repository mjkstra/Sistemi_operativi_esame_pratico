#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>

#define MAX_CHILDREN 100

pid_t root;
FILE* file;
int CHILDREN = 0;
pid_t children[MAX_CHILDREN];
void checkFile(const char*);
void serverHandle(int sigNum, siginfo_t info, void* context);
void clientHandle(int sigNum, siginfo_t info, void* context);
int main(int argc, char** argv){

    if (argc != 3){
        fprintf(stderr, "Invalid usage\n");
        exit(1);
    }
    checkFile(argv[2]);

    file = fopen(argv[2], "w+");

    if (file == NULL){
        perror("File opening error:");
        exit(2);
    }

    if (strcmp(argv[1], "server") == 0){
        fprintf(file, "%d\n", root = getpid()); fflush(file);
        //resta in attesa
        signal(SIGINT, serverHandle);
        signal(SIGUSR1, serverHandle);
        signal(SIGUSR2, serverHandle);
        while (1);
        
    } else if (strcmp(argv[1], "client") == 0){
        pid_t server;
        while (fscanf(file, "%d\n", &server) == 0);
        printf("[client] server:%d", server);
    } else {
        fprintf(stderr, "Invalid parameter! Use <client> or <server> \n");
        exit(3);
    }

    return 0;
}

void checkFile(const char* file){
    int fd = open(file, O_RDONLY, 777);
    if (fd > 0){
        fprintf(stderr, "File already exists!\n"); fflush(stderr);
        exit(4);
    }
}

void serverHandle(int sigNum, siginfo_t info, void* context){
    if (sigNum == SIGUSR1){
        if (getpid() == root){
            printf("[server:%d]\n", root);
            int id = fork();
            if (id < 0){
                perror("Forking error on SIGUSR1\n");
                exit(5);
            }
            CHILDREN++;
            if (id){
                children[CHILDREN++] = id;
                fprintf(file, "+%d\n", id); fflush(file);
                printf("[server]+%d\n", id);
            }
        }
    } else if (sigNum == SIGUSR2){
        if (CHILDREN > 0){
            fprintf(file, "-%d\n", children[CHILDREN-1]); fflush(file);
            printf("[server]-%d\n", children[CHILDREN-1]);
            if (kill(children[CHILDREN-1], SIGINT) == -1){
                perror("Killing on SIGUSR2 error:");
                exit(7);
            }
            children[CHILDREN-1] = 0;
            CHILDREN--;
        } else {
            fprintf(file, "0\n"); fflush(file);
            printf("[server]0\n");
        }
    } else if (sigNum == SIGINT){
        fprintf(file, "%d\n", CHILDREN); fflush(file);
        for(int i=0; i<CHILDREN; ++i){
            kill(children[i], SIGINT);
        }
        exit(0);
    } else {
        printf("Unexpected signal.\nUse SIGINT SIGUSR1 SIGUSR2\n");
    }
}

void clientHandle(int signNum, siginfo_t info, void* context){

}