#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <string.h>

struct qmsg{
    long int type;
    char buf[256];
};

int main(){

    key_t qKey = ftok("log.txt", 0);
    int queueID = msgget(qKey, 0777 | IPC_CREAT);
    if (queueID == -1){
        perror("msgGet error");
        if (msgctl(queueID, IPC_RMID, NULL)==-1){
            perror("Unsuccessful MQueue removal");
            exit(1);
        }
        queueID = msgget(qKey, 0777 | IPC_CREAT);

    }
    struct qmsg msg;
    int id = fork();
    if (id){
        msg.type = 1;
        strcpy(msg.buf, "diocane");
        if (msgsnd(queueID, &msg, sizeof(msg.buf), 0) == -1){
            perror("Msg Send error");
            exit(3);
        }
    } else { 
        if (msgrcv(queueID, &msg, sizeof(msg.buf), msg.type, 0) == -1){
            perror("Msg receive error");
            exit(4);
        }
        printf("%s\n", msg.buf);
    }


    return 0;
}