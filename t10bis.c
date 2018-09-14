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
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define PAGESIZE 4096
#define NUMPAGES 2


char *addr;
FILE* file;
int ret,k;
sem_t* semp,*semf;


void handler(){
	printf("\nSIGINT received\n");
	//scelta intervallo 10 chars
	exit(0);
	}

void routine(){
	
	while(1){
	
		printf("sono figlio vengo bloccato da sem2\n");
		
		ret = sem_wait(semf);
		
		if(ret == -1){
			printf("sem_wait fail!\n");
			exit(-3);
			}
	
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
		fflush(stdin);
		sleep(1);
	
		printf("\nMessage '%s' succesful wrote on file!\n",addr);
		
		ret = sem_post(semp);
		
		if(ret == -1){
			printf("sem_post fail!\n");
			exit(-3);
			}

	
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
		
	addr = (char*)mmap(NULL,NUMPAGES*PAGESIZE,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,0,0);
	
	if(addr == ((void*)-1)){
		printf("mmap fail!\n");
		exit(-3);
		}
		
	semp = sem_open("father",O_CREAT,0666,1);
	sem_unlink("father");
	
	if(semp == SEM_FAILED){
		printf("sem_open fail!\n");
		exit(-3);
		}
	semf = sem_open("child",O_CREAT,0666,0);
	sem_unlink("child");
	
	if(semf == SEM_FAILED){
		printf("sem_open fail!\n");
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
	
	int i;
	printf("doing fork %d times\n",N);
	
	for(i=0;i<N;i++){
		if(fork() == 0){
			routine();
			}
		
		} 
		
		signal(SIGINT,handler); //errori signal
		
		while(1){
		
		
			
			printf("sono il padre\n");
			
			ret = sem_wait(semp);
		
			if(ret == -1){
				printf("sem_wait fail!\n");
				exit(-3);
				}
				
			printf("insert a string please, it will be written on file named '%s'\n",argv[2]);
			
			if(scanf("%s",addr) == EOF){
				printf("scanf fail!\n");
				exit(-7);
				}
			
				sleep(1);
				fflush(stdin);
				printf("...\n");
				sleep(1);
				fflush(stdin);
				printf("Message '%s' received!\n",addr);

				
				ret = sem_post(semf);
		
				if(ret == -1){
					printf("sem_post fail!\n");
					exit(-3);
			
					
				}
				
		}
		
		
		
	
		
	printf("KILL %d\n",getpid());

			
	return 0;
	
}
		
	