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

//struttura data
struct data{
    char string[128];
    int pid;
    struct data *next;
};

sem_t *semP,*semF;
pid_t p;
struct data *head;
char *addr;
int nproc, shmid;

//funzione per inserimento nella struct
void push(struct data **head, char *str, int p){
    struct data *temp = malloc(sizeof(struct data));            //alloco struct temporanea
    temp->pid = p;                                              //assegno valore pid
    strcpy(temp->string,str);                                   //copio valore nella stringa    
    temp->next = *head;                                         //next punta a quello che puntava head
    *head = temp;                                               //head punta a temp
}

//stampa valori struttura
void print(){
    printf("\n");
    for(; head!=NULL;head=head->next){
        printf("stringa %s - processo %d\n",head->string,head->pid);
    }
    exit(0);
}

//funzione figli
void foo(){
    signal(SIGINT,print);
    while(1){
        sem_wait(semF);
        printf("inizio scrittura\n");
        p = getpid();
        push(&head,addr,(int)p);                //chiamo funzione push
        sem_post(semP);
    }
}

int main(int argc, char const *argv[])
{
    if(argc != 2) return -1;

    //creo semafori con nome
    semP = sem_open("padre",O_CREAT,0666,1);
    sem_unlink("padre");
    semF = sem_open("figlio",O_CREAT,0666,0);
    sem_unlink("figlio");

    shmid = shmget(1111,4096,IPC_CREAT|0666);
    addr = shmat(shmid,NULL,SHM_R|SHM_W);

    //genero N processi
    nproc = atoi(argv[1]);
    for(int i=0; i<nproc; i++){
        if(!fork()){
            foo(); 
        }
    }

    //prendo valore stringa
    while(1){
        sem_wait(semP);
        printf("inserisci stringa\n");
        scanf("%s",addr);
        sem_post(semF);
    }

    return 0;
}
