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
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>


sem_t *semp,*semf;


void routine(){
	while(1){
		sem_wait(semf);
		printf("sono figlio %d\n",getpid());
		sleep(2);
		fflush(stdin);
		sem_post(semp);
		}
		}

int main(int argc,const char* argv[]){

	if(argc < 3){
		printf("usage: nomeProg interoN nomeFile");
		exit(-1);
		}
		
	int N,i;
	
	N = atoi(argv[1]);
	
	semp = sem_open("father",O_CREAT,0666,1);
	//sem_unlink("father");
	if(semp == SEM_FAILED){
		printf("semp fail!\n");
		exit(-1);
		}
	 
	semf = sem_open("child",O_CREAT,0666,0);
	//sem_unlink("child");
	if(semf == SEM_FAILED){
		printf("semf fail!\n");
		exit(-1);
		}
	
	
	for(i=0;i<N;i++){
		if(fork() == 0){
			routine();
			}
		}
			
		while(1){
			sem_wait(semp);
			printf("so padre");
			sleep(2);
			fflush(stdin);
			sem_post(semf);
			}
	return 0;
	}
	
	
	