/*
Programma C che prende come argomento un intero N > 1 ed un path. Da 
questo intero genera N processi figli. Ogni processo figlio a sua volta
deve scrivere all'interno del file specificato dal path il proprio PID
ed un valore a scelta da stdin (tra i due vi deve essere uno spazio) preso dal padre.
Il programma inoltre dovrà supportare la ricezione del segnale 
SIGINT, in particolare quando il segnale verrà ricevuto da stdin sara'
possibile scegliere un intervallo per leggere esattamente 10 spazi all
interno del file.
*/
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/ipc.h>

char output[4096];
int fd, nproc, shmid, *value;
sem_t *semp, *semf;
FILE *file;

//funzione stampa(parte quando CTRL-C è stato premuto)
void printer(){
    size_t byter;                                     //variabile che contiene num byte letti
    printf("\n");
    rewind(file);                                     //indice file lo posiziono all'inizio per scansionare
    while((byter = fread(output,1,4096,file)) > 0)    //finche leggo byte li metto in output
        fwrite(output,1,byter,stdout);                //scrive su stdout
    fflush(stdout);                                   //flush stdout: senno nn hai niente
    printf("\n");
    //io ho messo di terminare
    exit(0);
}

//funzione figlio
void foo(){
    signal(SIGINT,SIG_IGN);

    //while figlio
    while(1){
        sem_wait(semf);
        sleep(1);
        printf("child - %d\n",getpid());
        fprintf(file, "value: %d - process: %d /", *value, getpid());   //scrittura del valore da scanf - pid del processo che ha scritto
        fflush(file);
        sem_post(semp);
    }
}

int main(int argc, char const *argv[])
{
    //controllo se sono esattamente 3 argc
    if(argc != 3){
        printf("syntax error\n");
        return -2;
    }

    shmid = shmget(1111,4096,IPC_CREAT|0666);
    value = (int*)shmat(shmid,NULL,SHM_R|SHM_W);

    //manca gestione SIGINT
    signal(SIGINT,printer);

    nproc = atoi(argv[1]);  //converto string a int

    //apertura file
    fd = open(argv[2],O_CREAT|O_TRUNC|O_RDWR,0666); 
    file = fdopen(fd,"r+");

    semp = sem_open("father",O_CREAT,0666,1);       //creo semaforo padre con nome in una shared memory
    sem_unlink("father");   //unlink in caso crash programma(senno rimane in memo)
    if(semp == SEM_FAILED){
        printf("failed sem - %d\n",errno);
        return -1;
    }

    semf = sem_open("child",O_CREAT,0666,0);        //creo semaforo figlio con nome in una shared memory
    sem_unlink("child");    //unlink in caso crash programma(senno rimane in memo)
    if(semf == SEM_FAILED){
        printf("failed sem - %d\n",errno);
        return -1;
    }

    //creazione processi
    for(int i=0; i<nproc; i++){
        if(!fork()){
            foo();
        }
    }

    //while proc padre
    while(1){
        sem_wait(semp);
        sleep(1);
        printf("daddy\n");
        scanf("%d",value);  //prendo valore
        sem_post(semf);
    }

    sem_close(semf);
    sem_close(semp);
}
