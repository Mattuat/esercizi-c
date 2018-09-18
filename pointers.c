


#include  <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <pthread.h>
#include <signal.h>

#define SIZE 100

char **messaggio;
int N;

void* tfunc(void* dummy){
	long sos;
	sos =(long)dummy;
	printf("parola %ld thread processo figlio\n",sos);
	printf("%s\n",messaggio[sos]);
	kill(getpid(),SIGTERM);
	}

int printer(){
	int i;
	i = 0;
	for(i=0;i<N-1;i++){
		printf("parola %d processo figlio\n",i);
		printf("%s\n",messaggio[i]);
		}
	pthread_t t;
	long j = N-1;
	pthread_create(&t,NULL,tfunc,(void*)j);
	pause();
	}
	

int main(int argc,char const* argv[]){
	
	N = 5;
	int i;
	messaggio = (char**)mmap(NULL,sizeof(char*)*N,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,0,0);
	
	for(i=0;i<N;i++){
		messaggio[i] = (char*)mmap(NULL,sizeof(char)*SIZE,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,0,0);
		}
		
	
	for(i=0;i<N;i++){
		printf("inserire parola\n");
		scanf("%s",messaggio[i]);
		}
		
	if(fork() == 0){
		printer();
		}
	wait(&i);
	printf("goodbye\n");	
	return 0;
	}