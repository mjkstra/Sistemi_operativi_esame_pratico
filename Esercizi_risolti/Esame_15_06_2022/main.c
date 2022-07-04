#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define CMD_LENGTH 32+1
#define SYS_CMD_LENGTH 30 + CMD_LENGTH*2 + 1
#define MSG_LENGTH 50+1
#define DEBUG 0

typedef enum { FALSE=0, TRUE=1 } bool;
int pid_dest = -1;

int get_queue( char * queue_name, bool verbose, bool exit_on_new );
bool file_exists( char * queue_name );
void quit( int errNo );
void path_creator( char* original_path ); // vedo se esiste il percorso altrimenti lo creo

int main(int argc, char* argv[]){
    char queue_name[CMD_LENGTH], action[CMD_LENGTH], value[CMD_LENGTH];

    if(argc < 4)
        quit(1);
    strcpy(queue_name, argv[1]);
    strcpy(action, argv[2]);
    
    if( !strcmp(action,"new") ){
        
        pid_dest = atoi(argv[3]);
        get_queue(queue_name, TRUE, FALSE);

    }else if( !strcmp(action,"put") ){
        if(argc != 5)
            quit(1);
        strcpy(value, argv[3]);
        pid_dest = atoi(argv[4]);

        int queue_id = get_queue(queue_name, FALSE, FALSE);
        if(msgsnd(queue_id,value,sizeof(value),IPC_NOWAIT) == -1)
            quit(4); 
        
    }else if( !strcmp(action,"get") ){
        pid_dest = atoi(argv[3]);
        char msg[MSG_LENGTH];

        int queue_id = get_queue(queue_name,FALSE, FALSE);
        
        if(msgrcv(queue_id,msg,sizeof(msg),0,IPC_NOWAIT) != -1)
            printf("%s\n", msg);
        
    }else if( !strcmp(action,"del") ){
        pid_dest = atoi(argv[3]);
        if(msgctl(get_queue(queue_name,FALSE, FALSE),IPC_RMID,NULL) == -1)
            quit(6);
        
    }else if( !strcmp(action,"emp") ){
        pid_dest = atoi(argv[3]);
        char msg[MSG_LENGTH];
        int queue_id = get_queue(queue_name,FALSE, FALSE);

        while(msgrcv(queue_id,msg,sizeof(msg),0,IPC_NOWAIT) != -1)
            printf("%s\n",msg);
        
    }else if (!strcmp(action, "mov")){
        if(argc != 5)
            quit(1);
        strcpy(value, argv[3]);
        pid_dest = atoi(argv[4]);
        char msg[MSG_LENGTH];
        int queue_id1 = get_queue(queue_name,FALSE,TRUE), queue_id2 = get_queue(value,FALSE,FALSE),count_msg;

        for(count_msg=0;msgrcv(queue_id1,msg,sizeof(msg),0,IPC_NOWAIT) != -1;count_msg++){
            printf("%s\n",msg);
            if(msgsnd(queue_id2,msg,sizeof(msg),IPC_NOWAIT) == -1)
                quit(4); 
        }
        printf("%d\n",count_msg);

        msgctl(queue_id1,IPC_RMID,NULL);

    }else
        quit(1);

    if( pid_dest >= 0 ){
        if(kill(pid_dest, SIGUSR1) == -1)
            quit(2);
    }else
        quit(1);

    return 0;
}

bool file_exists( char * queue_name ){
    bool esito = FALSE;
    int fd =  open(queue_name, O_RDONLY);
    if ( fd > 0 ){
        close(fd);
        esito = TRUE;
    }
    return esito;
}

void path_creator( char* original_path ){
    char system_command[SYS_CMD_LENGTH], path[CMD_LENGTH], token[CMD_LENGTH]="";
    char* temp;
    long long int last_path;

    strcpy(path,original_path);
    last_path = strrchr(path,'/') - path;
    (last_path >= 0) ? (path[last_path] = '\0') : (path[0] = '\0');
    temp = strtok(path, "/");
    
    while( temp != NULL ){
        strcat(token,temp);
        strcat(token,"/");
        //oppure così sprintf(token, "%s%s/", token,temp);
        sprintf(system_command, "[ ! -e %s ] && mkdir %s", token,token);
        if(DEBUG)
            fprintf(stderr,"%s\n", system_command);
        system(system_command);
        temp = strtok(NULL, "/");   
    }
}

int get_queue(char *queue_name,bool verbose, bool exit_on_new){

    key_t queue_key;
    int queue_id;

    if ( !file_exists(queue_name) ){
        if( creat(queue_name, 0777) == -1 )
            quit(3);
    }

    queue_key = ftok(queue_name, 1);
    queue_id = msgget(queue_key, 0777 | IPC_CREAT | IPC_EXCL);

    if( queue_id == -1 ){
        queue_id = msgget(queue_key, 0777 );
        if( verbose )
            printf("Already existing queue (not created): %d\n", queue_id);
    }else if( exit_on_new )
        quit(5);
    
    return queue_id;
}

void quit(int errNo){

    switch (errNo){
        case 1:
            fprintf(stderr, "Incorrect parameters !\n\
    Usage: ./coda <name> <action> [<value>] <pid>\n\
    Note: <value> must be provided only when <action> = put | mov\n\
    Note: <action> = new | put | get | del | emp | mov\n\
    Note: <pid> must be a positive number\n");
            break;
        case 2:
            fprintf(stderr, "Kill error\n");
            exit(2);
            break;
        case 3:
            fprintf(stderr, "Creat error\n");
            break;
        case 4:
            fprintf(stderr, "The queue is full\n");
            break;
        case 5:
            fprintf(stderr, "Mov error\n");
            break;
        case 6:
            fprintf(stderr, "Del error\n");
            break;
        default:
            fprintf(stderr, "Unknown error\n");
            break;
    }

    if( pid_dest >= 0 )
        if( kill(pid_dest, SIGUSR2) == -1 )
            quit(2);
    
    exit(errNo);
}


