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
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <string.h>
#include <errno.h>

#define PAGESIZE 4096
#define NUMPAGES 2


char *addr;
FILE* file;
sem_t* sem1;
sem_t* sem2;
int ret,k;



void handler(){
	printf("\nSIGINT received\n");
	//scelta intervallo 10 chars
	exit(0);
	}

void routine(){

	while(1){
	printf("sono figlio vengo bloccato da sem2\n");
		
	sem_wait(sem2); //figlio entra in sez critica
	
	printf("process %d writing '%s' on file",getpid(),addr);
	
	for(k=0;k<3;k++){
		sleep(1);
		printf(".");
		fflush(stdin);
		}
		
	ret = fprintf(file,"%d ",getpid());
	
	if(ret <= 0){
		printf("write error!\n");
		exit(-4);
		}
		
	ret = fprintf(file,"%s ",*addr);
	
	if(ret <= 0){
		printf("write error!\n");
		exit(-4);
		}
	sleep(1);
	fflush(stdin);
	printf("\nMessage '%s' succesful writed on file!\n",addr);
	
	sem_post(sem1); //sblocco padre
	
	}
	exit(1);
	}
	
int main(int argc,const char* argv[]){

	if(argc < 3){
		printf("usage: nomeProgramma interoN nomeFile\n");
		exit(-1);
		}
	int N,fd;
	
	N = atoi(argv[1]);
	
	if(N <= 1){
		printf("insert interoN > 1\n");
		exit(-2);
		}
		
	addr = mmap(NULL,NUMPAGES*PAGESIZE,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,0,0);
	
	if(addr == ((void*)-1)){
		printf("mmap fail!\n");
		exit(-3);
		}
	
	sem1 = mmap(NULL,5*PAGESIZE,PROT_READ|PROT_WRITE|PROT_EXEC,MAP_SHARED|MAP_ANONYMOUS,0,0);
	
	if(sem1 == ((void*)-1)){
		printf("mmap fail!\n");
		exit(-3);
		}
	
	sem2 = mmap(NULL,5*PAGESIZE,PROT_READ|PROT_WRITE|PROT_EXEC,MAP_SHARED|MAP_ANONYMOUS,0,0);
	
	if(sem2 == ((void*)-1)){
		printf("mmap fail!\n");
		exit(-3);
		}
		
	fd = open(argv[2],O_CREAT|O_TRUNC|O_RDWR,0666);
	
	if(fd == -1){
		printf("open fail!\n");
		exit(-4);
		}
		
	file = fdopen(fd,"r+");
	
	if(file == NULL){
		printf("fdopen fail!\n");
		exit(-5);
		}
	
	ret = sem_init(sem1,1,1); //semaforo padre
	
	if(ret == -1){
		printf("sem_init fail!\n");
		exit(-6);
		}
		
	ret = sem_init(sem2,1,0); //semaforo figli
	
	if(ret == -1){
		printf("sem_init fail!\n");
		exit(-6);
		}


	signal(SIGINT,handler); //errori signal
	
	int i;
	printf("doing fork %d times\n",N);
	
	for(i=0;i<N;i++){
		if(fork() == 0){
			routine();
			}
		
		else{ 
			while(1){
			
			sem_wait(sem1); //padre entra nella sez critica
		
			printf("insert a string please, it will be written on file named '%s'\n",argv[2]);
			
			if(scanf("%s",addr) == EOF){
				printf("scanf fail!\n");
				exit(-7);
				}
			else{
				sleep(1);
				fflush(stdin);
				printf("...\n");
				sleep(1);
				fflush(stdin);
				printf("Message '%s' received!\n",addr);
				
				sem_post(sem2); //sblocco figlio
				}
				
			}
		
		}
		}
	
		
	printf("KILL %d\n",getpid());

			
	return 0;
	
}
		
	