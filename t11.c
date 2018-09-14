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

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

#define MAXSIZE 1024


char messaggio[MAXSIZE];
sem_t *semf,*semp;
int i;
int *fd;


void* routine(){
		int j;
		ssize_t ret;
		j = i-1;
			
		while(1){
			 
			 sem_wait(semf);
			 
			 printf("thread %ld is in!\n",pthread_self());
			 
			 ret = write(*(fd+j),messaggio,strlen(messaggio));
			 
			 if(ret == -1 || ret < strlen(messaggio)){
			 	printf("write error\n");
			 	exit(-1);
			 	}
			 	
			 sem_post(semp);
			 }
		exit(-1);
		}

int main(int argc,const char* argv[]){

	if(argc < 2){
		printf("usage: nomeProg nomeFile1 nomeFile2 ... nomeFileN\n");
		exit(-1);
		}

	int N;
	N = argc-1;
	
	int ret;
	
	
	fd = malloc(sizeof(int)*N);
	
	if(fd == NULL){
		printf("malloc error!");
		exit(-1);
		}
		
	semp = malloc(sizeof(sem_t));
	if(semp == NULL){
		printf("malloc error!");
		exit(-1);
		}
		
	semf = malloc(sizeof(sem_t));
	if(semf == NULL){
		printf("malloc error!");
		exit(-1);
		}
		
	ret = sem_init(semf,0,0);
	
	if(ret == -1){
	
		printf("sem_init fail!\n");
		exit(-1);
		
		}
	ret = sem_init(semp,0,1);
	
	if(ret == -1){
	
		printf("sem_init fail!\n");
		exit(-1);
		
		}  

	pthread_t thread[N];
	
	printf("creating %d threads and %d files\n",N,N);
	
	for(i=0;i<N;i++){
		
		*(fd + i) = open(argv[i+1],O_TRUNC|O_CREAT|O_RDWR,0666);
		
		if(*(fd+i) == -1){
		printf("open fail!\n");
		exit(-1);
		}
		
		ret = pthread_create(&thread[i],NULL,routine,NULL);
		
		if(ret != 0){
			printf("pthread_create error!\n");
			exit(-1);
			}
		
		}
		
	
		
	while(1){
	
		sem_wait(semp);
		printf("\ntype word\n");
		scanf("%s",messaggio);
		sem_post(semf);
		
		}
		
	return 0;
}	
	
		
		