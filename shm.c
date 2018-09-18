#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <string.h>

int shmid;
void *old, *shared;

struct list{
    int a;
    char b[128];
}*list;

void push(){
    struct list s;
    printf("\nimmetti decimale: ");
    scanf("%d",&s.a);
    printf("\n");
    printf("immetti stringa: ");
    scanf("%s",s.b);
    printf("\n");
    memcpy(shared, &s, sizeof(struct list));
    
    printf("valore dec: %d - valore string: %s\n", *(int*)shared, (char*)(shared+4));
    shared = shared + 5;
}

int main(int argc, char const *argv[])
{
    int i = 0;

    shmid = shmget(11,sizeof(list),IPC_CREAT|0666);
    shared = shmat(shmid,NULL,SHM_R|SHM_W);
    old = shared;

    push();
    push();
    push();

    printf("\n");
    while(i < 3){
        printf("valore dec: %d - stringa: %s\n", *(int*)old, (char*)(old+4));
        old = old + 5;
        i++;
    }

    return 0;
}
