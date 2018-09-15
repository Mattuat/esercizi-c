/*Programma C che prende in input un intero N > 1 dal quale crea N figli. 
I figli andranno a scrivere all'interno di una linked list il valore
del proprio pid ed una stringa. Il programma dovrà supportare la ricezione
del segnale SIGINT. In particolare quando sarà ricevuto il segnale
dovrà essere riversato l'intero contenuto della linked list con 
l'associazione PID -> Stringa -> PID -> Stringa..*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/ipc.h>

struct data{
    char *string;
    int pid;
    struct data *next;
};

sem_t *semP,*semF;
pid_t p;
struct data *head;
char *addr;
int nproc, shmid, pWrite;

void push(struct data **head, char *str, int p){
    struct data *temp = malloc(sizeof(struct data));
    temp->pid = p;
    temp->string = str;
    temp->next = *head;
    *head = temp;
    memset(addr,0,sizeof(addr));
}

void printer(){
    exit(0);
}

void foo(){
    signal(SIGINT,printer);
    while(1){
        sem_wait(semF);
        p = getpid();
        printf("inizio scrittura\n");
retry:
        if(!strcmp(addr,"")) goto retry;
        pWrite = 1;
        sem_post(semP);
    }
}

int main(int argc, char const *argv[])
{
    head = malloc(sizeof(struct data));

    if(argc != 2) return -1;

    signal(SIGINT,SIG_IGN);

    semP = sem_open("padre",O_CREAT,0666,1);
    sem_unlink("padre");
    semF = sem_open("figlio",O_CREAT,0666,0);
    sem_unlink("figlio");

    shmid = shmget(1111,4096,IPC_CREAT|0666);
    addr = shmat(shmid,NULL,SHM_R|SHM_W);

    nproc = atoi(argv[1]);
    for(int i=0; i<nproc; i++){
        if(!fork()){
            foo(); 
        }
    }

    int j = 0;
    while(!j){
        sem_wait(semP);
        if(pWrite) push(&head,addr,(int)p);
        printf("inserisci stringa\n");
        scanf("%s",addr);
        pWrite = 0;
        if(!strcmp(addr,"print")) j=1;
        sem_post(semF);
    }

    for(;head != NULL; head = head->next)
        printf("string %s - processo %d\n",head->string,head->pid);

    return 0;
}
