#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#define false 0
#define true 1
#define MAX_LENGTH 256

typedef int bool;

bool L_OPTION = false;
bool C_OPTION = false;
bool F_OPTION = false;
int main(int argc, char** argv){
    int fd;
    if (argc > 4){
        fprintf(stderr, "Inappropriate usage!\n");
        exit(1);
    }    
    int a = 1;
    while(a++ < argc){
        int b = strlen(argv[a]);
        if (argv[a][0] == '-'){
            if (argv[a][1] == 'l') L_OPTION = true;
            else if (argv[a][1] == 'c') C_OPTION = true;
            else {
                fprintf(stderr, "Unexpected value! Only use '-l' or '-c\n");
                exit(2);
            }
        }
        if (argv[a][b-4] == '.' && argv[a][b-3] == 't'  && argv[a][b-2] == 'x'
         && argv[a][b-1] == 't'){
            printf("There's a file\n");
            F_OPTION = true;
            fd = open(argv[a], O_RDONLY);
        } else {
            printf("There's no file\n");
        }
    }
    int id = fork();
    char l[] = "-l";
    char c[] = "-c";
    if (F_OPTION){
        dup2(fd, 0);
        close(fd);
    }
    if (!id){
        if (L_OPTION && C_OPTION){
            execlp("wc", c, l, NULL);
        } else if (L_OPTION){
            execlp("wc", l, NULL);
        } else if (C_OPTION){
            execlp("wc", c, NULL);
        } else {
            fprintf(stderr, "Something went wrong\n");
        }
    }
}