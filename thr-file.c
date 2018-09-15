/*Implementare un programma che riceva in input tramite argv[] i pathname 
associati ad N file, con N maggiore o uguale ad 1. Per ognuno di questi
file generi un thread (quindi in totale saranno generati N nuovi thread 
concorrenti). 
Successivamente il main-thread acquisira' stringhe da standard input in 
un ciclo indefinito, ed ognuno degli N thread figli dovra' scrivere ogni
stringa acquisita dal main-thread nel file ad esso associato.
L'applicazione dovra' gestire il segnale SIGINT (o CTRL_C_EVENT nel caso 
WinAPI) in modo tale che quando uno qualsiasi dei thread dell'applicazione
venga colpito da esso dovra' stampare a terminale tutte le stringhe gia' 
immesse da standard-input e memorizzate nei file destinazione.*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/ipc.h>

sem_t sem;
int shmid, *fd, nthread, counter = 0;
char *string, output[4096];
FILE *file;

//funzione stampa: stampa tutti i file relativi ai thread
void printer(){
    size_t bread;                                       //byte letti
    for(int i=0; i<nthread; i++){
        printf("\n");
        file = fdopen(fd[i],"r+");                      //apertura file i-esimo
        rewind(file);                                   //indice lett/scritt posizionato all'inizio
        while((bread = fread(output,1,4096,file)) > 0)  //finche leggo byte ciclo
            fwrite(output,1,bread,stdout);              //scrivo contenuto file sullo stdout
        fflush(stdout);                                 //flush dello stdout
        printf("\n");
    }
    exit(0);
}

//funzione figlio: parametro(descrittore file i-esimo)
void *fooM(void *f){
    int i=0;
    int tf = *(int*)f;                                      //descrittore file

    signal(SIGINT,printer);                                 //se ricevo segnale -> printer

    while(1){
        sem_wait(&sem);                             
        sleep(2);
        file = fdopen(tf,"r+");                             //apertura file
        printf("thread - %ld\n",pthread_self());
retry:
        if(!strcmp(string,"")) goto retry;                  //se contenuto string è vuoto ciclo
        fprintf(file,"%s - %ld/ ",string, pthread_self());  //scrittura su file i-esimo
        fflush(file);
        memset(string,0,sizeof(string));                    //svuoto la stringa
        sem_post(&sem);                             
        sleep(1);
    }
}

int main(int argc, char const *argv[])
{
    pthread_t thread[argc-1];

    signal(SIGINT,SIG_IGN);                                 //padre ignora segnale SIGINT

    if(argc < 2) return -1;
    nthread = argc-1;
    
    fd = (int*)malloc((argc-1)*sizeof(int));                //alloco spazio sufficiente per tutti i file
    
    shmid = shmget(1111,4096,IPC_CREAT|0666);               //creo memoria condivisa per stringa
    string = (char*)shmat(shmid,NULL,SHM_R|SHM_W);

    sem_init(&sem,0,1);

    //creazione thread e file
    for(int i=0; i<argc-1; i++){                           
        fd[i] = open(argv[i+1],O_CREAT|O_TRUNC|O_RDWR,0666);
        pthread_create(&thread[i],NULL,fooM,(void*)&fd[i]);
    }

    //while padre
    while(1){
        sleep(2);
        printf("immetti un valore\n");
        scanf("%s",string);
    }

    return 0;
}
