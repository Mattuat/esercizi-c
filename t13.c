/*
Implementare un programma che riceva in input tramite argv[2] un 
numero intero N maggiore o uguale ad 1 (espresso come stringa di 
cifre decimali), e generi N nuovi processi. Ciascuno di questi 
leggerà in modo continuativo un valore intero da stdin e lo 
comunicherà al processo padre tramite memoria condivisa. Il 
processo padre scriverà ogni nuovo valore intero ricevuto su 
di un file, come sequenza di cifre decimali. I valori scritti
su file devono essere separati dal carattere ' ' (blank).
Il pathname del file di output deve essere comunicato all'appl
tramite argv[1].

L'applicazione dovrà gestire il segnale SIGINT in modo tale che 
se il proceso padre venga colpito il contenuto del file di 
output venga interamente riversato su stdout.
Nel caso in cui non vi sia immissione di input, l'applicazione
non deve consumare più del 5% della capacità di lavoro della CPU.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <errno.h>
#include <signal.h>
#include <string.h>

#define MAXREAD 20

int *messaggio;
sem_t *semfa;
sem_t *semch;
int fd,ret;
FILE* file;

void print(){
	printf("\nSIGINT received print of file:\n");
	size_t size;
	char output[20];
	rewind(file);
	while((size = fread(output,1,MAXREAD,file)) > 0 ){
		fwrite(output,1,size,stdout);
		}
	fflush(stdout);
	printf("\n");
	exit(1);
	}
void routine(){
		signal(SIGINT,SIG_IGN);
		if(errno == EINVAL){
		printf("signal error!\n");
		exit(-1);
		}
		while(1){
			sem_wait(semch);
			
			printf("type an integer for process %d\n",getpid());
			
			scanf("%d",messaggio);
			
			sem_post(semfa);
			}
}


int main(int argc,const char* argv[]){

	if(argc < 3){
		printf("usage: programName fileName integerN\n");
		exit(-1);
		}
		
	int N,i,ret;
	
	N = atoi(argv[2]);
	
	if(N <= 0){
		printf("type a number greather than 0 please\n");
		exit(-1);
		}
		
	messaggio = mmap(NULL,sizeof(int),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,0,0);
	if(messaggio == NULL){
		printf("mmap error!\n");
		exit(-1);
		}
		
	sem_unlink("Father");
	semfa = sem_open("Father",O_CREAT|O_EXCL,0666,0);
	if(semfa == SEM_FAILED){
		printf("sem_open error!\n");
		sem_close(semfa);
		exit(-1);
		}
		
	sem_unlink("Child");
	semch = sem_open("Child",O_CREAT|O_EXCL,0666,1);
	if(semch == SEM_FAILED){
		printf("sem_open error!\n");
		sem_close(semch);
		exit(-1);
		}
		
	for(i=0;i<N;i++){
		if(fork()==0){
			routine();
			}	
		}
		
	signal(SIGINT,print);
	if(errno == EINVAL){	//controllo signal
		printf("signal error!\n");
		exit(-1);
		}
	
	fd = open(argv[1],O_RDWR|O_CREAT|O_TRUNC,0666);
	char temp[20];//buffer per contenere l'intero trasformato in stringa
	
	if(fd == -1){
		printf("open error!\n");
		exit(-1);
		}
		
	file = fdopen(fd,"r+");

	while(1){
		sem_wait(semfa);
		ret = snprintf(temp,20,"%d",*messaggio); //funzione che trasforma int in stringa
		if(ret <= 0){
			printf("snprintf error!\n");
			exit(-1);
			}
			
		ret = fprintf(file,"%s ",temp);
		if(ret <= 1){
			printf("fprintf error!\n");
			exit(-1);
			}
			
		printf("Father process wrote on file\n");
		sem_post(semch);
	}
	return 0;		
}
		
		
		
		