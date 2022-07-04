#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

char* params[256];
int paramIndex=0;
FILE* file = NULL;
int fd;
int c_option = 0;
int l_option = 0;
int f_option = 0;

int main(int argc, char** argv){
    if (argc < 2 || argc > 4){
        fprintf(stderr, "Inappropriate usage!Define at least one parameters and a file(optional)\n");
        fflush(stderr);
        exit(1);
    }
    printf("Looking for arg1\n");
    if (argv[1][0] == '-'){
        params[paramIndex++] = argv[1];
        if (argv[1][1] == 'c'){
            c_option = 1;
        } else if (argv[1][1] == 'l'){
            l_option = 1;
        } else {
            fprintf(stderr, "Invalid parameter use only -l or -c\n");
            fflush(stderr);
            exit(3);
        }
    }
    printf("Looking for arg2\n");

    if (argc > 2 && argv[2][0] == '-'){
        params[paramIndex++] = argv[2];
        if (argv[2][1] == 'c'){
            c_option = 1;
        } else if (argv[2][1] == 'l'){
            l_option = 1;
        } else {
            fprintf(stderr, "Invalid parameter use only -l or -c\n");
            fflush(stderr);
            exit(3);
        }
    }

    printf("Looking for arg3\n");
    if (argc-1 > paramIndex){
        paramIndex++;
        int a = strlen(argv[paramIndex])-4;
        if (strcmp(argv[paramIndex]+a, ".txt")!=0){
            perror("Argument is not a file!");
        }
        file = fopen(argv[paramIndex], "r");
        if (file == NULL){
            perror("File does not exist\n");
            exit(8);
        }
        fd = fileno(file);
        if (file < 0){
            perror("File opening error:");
            exit(3);
        }
        f_option = 1;
    } else {
        printf("No file specified\n");
    }
    printf("Obtained params\n");
    // dup file desc
    if (f_option){
        printf("Duplicating...\n");
        if (dup2(fd, 0) == -1){
            perror("Unexpected error\n");
            exit(9);
        }
    }
    // execution
    printf("Executing\n");
    if (c_option && l_option){
        printf("Printing with option -c -l...\n");
        if (execlp("wc", "wc", "-c", "-l", NULL) < 0){
            perror("Unexpected exec error\n");
            exit(5);
        }
    } else if (c_option){
        printf("Printing with option -c...\n");
        if (execlp("wc", "wc", "-c", NULL) < 0){
            perror("Unexpected exec error\n");
            exit(5);
        }
    } else if (l_option){
        printf("Printing with option -l...\n");
        if (execlp("wc", "wc", "-l", NULL) < 0){
            perror("Unexpected exec error\n");
            exit(5);
        }
    } else {
        fprintf(stderr, "Unexpected behaviour did not execute command\n");
        fflush(stderr);
    }

    fclose(file);
    return 0;
}
