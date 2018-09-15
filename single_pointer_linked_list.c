
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

struct linkedlist{
		int p;
		char messaggio[100];
		struct linkedlist *next;
		};
		
struct linkedlist *head;
int x;
char mex[100];

void print(){
	while(head != NULL){
		printf("\n%d %s\n",head->p,head->messaggio);
		head = head->next;
		}
	exit(1);
	}
	
void push(){
	struct linkedlist *temp = malloc(sizeof(struct linkedlist));
	temp->p = x;
	strcpy(temp->messaggio,mex);
	temp->next = head;
	head = temp;
	}
	

int main(int argc,const char* argv[]){

		
	
	
	head = NULL;
	signal(SIGINT,print);
	
	while(1){
	
		printf("inserire intero\n");
		scanf("%d",&x);
		printf("inserire parola\n");
		scanf("%s",mex);
		push();
		}
		
	return 0;
		
	}