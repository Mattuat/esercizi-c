/*scrivere un programma C che prende in input N >= 1 e un nome file
esso crea N processi.
i processi figli prendono in input una stringa da stdin e la mettono in una struttura fatta come segue:
char messaggio[size]
int nr. processo [0...N-1]
puntatore a sè stessa
il padre dopo N inserimenti prende il contenuto della struttura e lo riversa nel file.
tutto con ciclo infinito, l'applicazione alla ricezione del SIGINT dovrà stampare il file a schermo
e terminare.*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#define SIZE 1024

sem_t *psem, *fsem;
int np, id, fd;
void *shared;
FILE *file;

struct data{
    char messaggio[SIZE];
    int nproc;
    struct data *next;
}*head;

void _push_write(struct data **head, int val, char *mex){
    struct data *temp = malloc(sizeof(struct data));
    strcpy(temp->messaggio,mex);
    temp->nproc = val;
    temp->next = *head;
    *head = temp;

    fprintf(file, "proc %d - string %s ",temp->nproc,temp->messaggio);
    fflush(file);
}

void foo(){
    char mex[SIZE];
    while(1){
        sem_wait(fsem);
        sleep(1);
        printf("\n\n");
        printf("immetti stringa: ");
        scanf("%s",mex);
        _push_write(&head,(int)getpid(), mex);
        sem_post(psem);
    }
}

int main(int argc, char const *argv[])
{
    char cmd[128];

    if(argc < 2) return -1;
    np = atoi(argv[1]);

    fd = open(argv[2],O_CREAT|O_TRUNC|O_RDWR,0666);
    file = fdopen(fd,"r+");

    head = mmap(NULL,4096,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,0,0);

    psem = sem_open("padre",O_CREAT,0666,1);
    sem_unlink("padre");
    fsem = sem_open("figlio",O_CREAT,0666,0);
    sem_unlink("figlio");

    for(int i=0; i<np; i++){
        if(!fork()) foo();
    }

    sprintf(cmd, "cat %s", argv[2]);

    while(1){
        sem_wait(psem);
        printf("\n");
        system(cmd);
        sem_post(fsem);
    }
    return 0;
}
