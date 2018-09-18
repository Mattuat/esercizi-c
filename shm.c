#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <string.h>
#include <fcntl.h>

int fd, shmid, cont;
int puntatori[3];
void *old, *shared;
FILE *file;

//struct
struct list{
    int a;
    char b[128];
};

//funzione push nella shared memory (nuovo elemento)
void push(){
    struct list s;
    size_t bytes;

    printf("\nimmetti decimale: ");
    scanf("%d",&s.a);
    printf("\n");
    printf("immetti stringa: ");
    scanf("%s",s.b);

    bytes = strlen(s.b);                            //quanti bytes sono stati scritti nella stringa?
    puntatori[cont] = (int)bytes;                   //array di lunghezze stringhe
    printf("\n");
    memcpy(shared, &s, sizeof(struct list));        //copio nella shared memory il nuovo elemento della struct
    
    shared = shared + 4 + (int)bytes;               //avanzo di 4bytes + bytes lunghezza array di char
    cont++;                                         //incrememnto contatore array delle lunghezze stringhe
}

int main(int argc, char const *argv[])
{
    int i = 0;

    if(argc != 2) return -1;

    fd = open(argv[1],O_CREAT|O_TRUNC|O_RDWR,0666);
    file = fdopen(fd,"r+");

    shmid = shmget(11,sizeof(struct list*),IPC_CREAT|0666);
    shared = shmat(shmid,NULL,SHM_R|SHM_W);
    old = shared;

    for(int i=0; i<3; i++) push();                  //alloco elemento struct

    printf("\n");
    while(i < 3){
        fprintf(file, "valore dec: %d - stringa: %.*s \n",*(int*)old, puntatori[i], (char*)(old+4));    //scrivo nel file
        fflush(file);                                                                                   //.* serve per decidere lunghezza di byte da leggere
        old = old + 4 + puntatori[i];
        i++;
    }

    char cmd[128];
    sprintf(cmd, "cat %s",argv[1]);
    system(cmd);    //stampo contenuto file

    return 0;
}
