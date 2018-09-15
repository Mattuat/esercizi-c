/*
Programma C che prende in input un intero N > 1 dal quale crea N figli. 
I figli andranno a scrivere all'interno di una linked list il valore
del proprio pid ed una stringa. Il programma dovrà supportare la ricezione
del segnale SIGINT. In particolare quando sarà ricevuto il segnale
dovrà essere riversato l'intero contenuto della linked list con 
l'associazione PID -> Stringa -> PID -> Stringa..
*/

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/mman.h>

#define MAXSIZE 50

struct linkedlist{
	long int pid;
	char messaggio[MAXSIZE];
	struct linkedlist* next;
	};
	
struct linkedlist *start;
struct linkedlist *temp;


sem_t *semp;
sem_t * semf;

char *mex;

void waitson(){
	int status;
	wait(&status);
	exit(1);
	}

void print(){

	while(temp != NULL){
			printf("pid: %ld -> %s\n",temp->pid,temp->messaggio);
			temp = temp->next;
		}
	exit(1);
	}

void* push(){
	start->pid = getpid();
	strcpy(start->messaggio,mex);
	temp = malloc(sizeof(struct linkedlist));
	if(temp == NULL){
		printf("malloc error!\n");
		exit(-1);
	}
	
	temp->next = start;
	start = temp;
	
	}

void routine(){
		printf("figlio cerca di entrare\n");
		signal(SIGINT,print);
		while(1){
			sem_wait(semf);
			printf("figlio %ld dentro\n",getpid());
			push();
			sem_post(semp);
			}
		}


int main(int argc,const char* argv[]){
	
	if(argc < 2){
		printf("usage: nomeProg interoN\n");
		exit(-1);
		}
	int N,i;
	N = atoi(argv[1]);
	
	if(N <= 1){
		printf("N must be greater than 1\n");
		exit(-1);
		}
		
	mex = mmap(NULL,sizeof(char)*MAXSIZE,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,0,0);
	if(mex == (void*)-1){
		printf("mmap error!\n");
		exit(-1);
		}
		
	start = malloc(sizeof(struct linkedlist));
	if(start == NULL){
		printf("malloc error!\n");
		exit(-1);
		}
		
	start->next = NULL;
	
	semp = mmap(NULL,sizeof(sem_t),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,0,0);
	if(semp == (void*)-1){
		printf("mmap error!\n");
		exit(-1);
		}
	
	semf = mmap(NULL,sizeof(sem_t),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,0,0);
	if(semf == (void*)-1){
		printf("mmap error!\n");
		exit(-1);
		}
		
	father:
	semp = sem_open("father",O_CREAT|O_EXCL,0666,1);
	if(semp == SEM_FAILED && errno == EEXIST){
		printf("semaphore semp created yet\n");
		sem_unlink("father");
		sem_close(semp);
		printf("retry creation\n");
		goto father;
		exit(-1);
		}
	else{
		printf("semp created!\n");
	     }
	
	son:
	semf = sem_open("child",O_CREAT|O_EXCL,0666,0);
	if(semf == SEM_FAILED && errno == EEXIST){
		printf("semaphore semf created yet\n");
		sem_unlink("child");
		sem_close(semf);
		printf("retry creation\n");
		goto son;
		exit(-1);
		}
	else{
		printf("semf created!\n");
	     }
	     
	
	for(i=0;i<N;i++){
		if(fork() == 0){
			routine();
			}
		}
	signal(SIGINT,waitson);
	while(1){
		sem_wait(semp);
		printf("inserire una stringa\n");
		scanf("%s",mex);
		sem_post(semf);
		}
		
	return 0;
	}
		
		
		