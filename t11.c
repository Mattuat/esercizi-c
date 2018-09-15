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
#include <signal.h>
#include <errno.h>

#define MAXSIZE 1024


char messaggio[MAXSIZE];
sem_t *semf,*semp;
int i;
int *fd;
int N;

void handler(){
	int k;
	FILE* file;
	size_t a;
	printf("SIGINT received by thread %ld\n",pthread_self());
	printf("printing files:\n\n");
	
	for(k=0;k<N;k++){
	
		file = fdopen(fd[k],"r+");
		
		printf("file nr %d content:\n",fd[k]-3);
		
		rewind(file);
		
		if(file == NULL){
			printf("fdopen error!\n");
			exit(-1);
			}
		printf("\n");
		while((a = fread(messaggio,1,MAXSIZE,file)) > 0){
			
			fwrite(messaggio,1,a,stdout);
			}
			fflush(stdout);
		printf("\n");
		printf("\n");
		}
		
	printf("end of print, program closed\n");
	
	free(fd);
	free(semp);
	free(semf);
	exit(1);
	}

void* routine(void* arg){
		int j;
		int *sos;
		ssize_t ret;
		sos = (int*)(arg);
		j = *sos -1;
		char c[1] = {" "};
		int rets;
		
		rets = signal(SIGINT,handler);
		if(rets == SIG_ERR || errno == EINVAL){
			printf("signal error!");
			exit(-1);
			}
		
		
			
		while(1){
			 
			 sem_wait(semf);
			 
			 printf("thread %ld is writing on file num %d!\n\n",pthread_self(),fd[j]-3);
			 printf("message is '%s'\n",messaggio);
			 
			 ret = write(fd[j],messaggio,strlen(messaggio));
			 if(ret == -1 || ret != strlen(messaggio)){
			 	printf("write1 error!\n");
			 	exit(-1);
			 	}
			 
			 ret = write(fd[j],c,1);
			 if(ret == -1 || ret != 1){
			 	printf("write2 error!\n");
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
		
		fd[i] = open(argv[i+1],O_TRUNC|O_CREAT|O_RDWR,0666);
		
		if(fd[i] == -1){
		printf("open fail!\n");
		exit(-1);
		}
		
		ret = pthread_create(&thread[i],NULL,routine,(void*) &i);
		
		if(ret != 0){
			printf("pthread_create error!\n");
			exit(-1);
			}
		}
		
	
		ret = signal(SIGINT,SIG_IGN);
		if(ret == SIG_ERR || errno == EINVAL){
			printf("signal error!");
			exit(-1);
			}
	while(1){
	
		sem_wait(semp);
		printf("\ntype word\n");
		scanf("%s",messaggio);
		sem_post(semf);
		
		}
		
	return 0;
}	
	
		
		