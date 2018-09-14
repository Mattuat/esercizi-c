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
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <string.h>


sem_t *semp,*semf;
char *addr;
FILE *file;
int fd;


void handler(){
	int start,end;
	off_t ret2,fileLenght;
	
	printf("stop signal received!\n");
	sleep(1);
	printf("choose range for reading:\n");
	
	init:
	printf("type initial value for range\n");
	scanf("%d",&start);
	
	final:
	printf("type final value for range (how many bytes to read)\n");
	scanf("%d",&end);
	
	fileLenght = lseek(fd,0,SEEK_END);
	
	ret2 = lseek(fd,start,SEEK_SET);
	
	if(ret2 > fileLenght || ret2 == (off_t)-1){
		printf("choose a valid value for the beginning of range please\n");
		goto init;
		}
	if((ret2 + end) > fileLenght){
		printf("can't read '%d' bytes: out of file!\n",end);
		goto final;
		}
	
	char readbuff[end];
	char *temp;
	
	ret2 = fread(readbuff,1,end,file);
	
	if(ret2 == 0){
		printf("read fail!\n");
		exit(-1);
		}
		
	printf("\n%s",readbuff);
	
		
	printf("\nProgram Closed, goodbye!\n");
	
	exit(1);
	}

void routine(){
	int ret,k;
	while(1){
		sem_wait(semf);
		
		sleep(1);
		
		printf("\nProcess %d is writing '%s' on file\n",getpid(),addr);
		
		ret = fprintf(file,"%d: %s\n",getpid(),addr);
		
		if(ret <= 0){
			printf("writing fail!\n");
			exit(-1);
			}
			
		fflush(file);
	
		for(k=0;k<3;k++){
			sleep(1);
			printf("...\n");
			fflush(stdin);
			}
		
		
		sleep(1);
		printf("Message wrote on file!\n");
		sleep(1);
		
		sem_post(semp);
		}
		}

int main(int argc,const char* argv[]){

	if(argc < 3){
		printf("usage: nomeProg interoN nomeFile\n");
		exit(-1);
		}
		
	int N,i,ret;
	
	N = atoi(argv[1]);
	
	semp = sem_open("father",O_CREAT,0666,1);
	sem_unlink("father");
	
	if(semp == SEM_FAILED){
		printf("semp_open fail!\n");
		exit(-1);
		}
	 
	semf = sem_open("child",O_CREAT,0666,0);
	sem_unlink("child");
	
	if(semf == SEM_FAILED){
		printf("semf_open fail!\n");
		exit(-1);
		}
		
	addr = (char*)mmap(NULL,4096*5,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,0,0);
	
	if(addr == (void*)-1){
		printf("mmap fail!\n");
		exit(-2);
		}
	
	fd = open(argv[2],O_CREAT|O_TRUNC|O_RDWR,0666);
	
	if(fd == -1){
		printf("open fail!\n");
		exit(-1);
		}
		
	file = fdopen(fd,"r+");
	
	if(file == NULL){
		printf("fdopen fail!\n");
		exit(-2);
		}
	
	for(i=0;i<N;i++){
		if(fork() == 0){
			routine();
			}
		}
			
		while(1){
			
			signal(SIGUSR1,handler);
			
			if(errno == EINVAL){
				printf("signal error!\n");
				exit(-1);
				}
			
			sem_wait(semp);
			
			printf("\ntype a word, it will be written on file '%s'\n",argv[2]);
			
			ret = scanf("%s",addr);
			
			if(ret == 0 || ret == EOF){
				printf("scanf fail!\n");
				exit(-1);
				}
				
			if(!strcmp(addr,"stop")){
				raise(SIGUSR1);
				}
			
			sem_post(semf);
			}
	return 0;
	}
	
	
	