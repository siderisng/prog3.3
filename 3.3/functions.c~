#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

void synch_beg();

void synch_wait();

void synch_notify();

void synch_notifyAll();

void synch_end();

void *FOO ();

pthread_mutex_t mtxMonitor, mtxBusy, mtxEnd, mtxBlock, check;
int ranVar;
int countWait, countBlock,thosePassed;

int main(){
	int i, thrCheck;
	pthread_t *pasThread; 
	
	thosePassed=1;
	countWait=0;
	ranVar=0;
	
	
	if (pthread_mutex_init(&check, NULL) != 0)	
	{
		perror ("Mutex error");
		return 1;
	}
	
	if (pthread_mutex_init(&mtxMonitor, NULL) != 0)	
	{
		perror ("Mutex error");
		return 1;
	}
	
	if (pthread_mutex_init(&mtxBusy, NULL) != 0)	
	{
		perror ("Mutex error");
		return 1;
	}
	pthread_mutex_lock (&mtxBusy);
	
	if (pthread_mutex_init(&mtxBlock, NULL) != 0)	
	{
		perror ("Mutex error");
		return 1;
	}
	pthread_mutex_lock (&mtxBlock);
	
	
	if (pthread_mutex_init(&mtxEnd, NULL) != 0)	
	{
		perror ("Mutex error");
		return 1;
	}
	pthread_mutex_lock (&mtxEnd);
	
	if (NULL==(pasThread= (pthread_t*)malloc(sizeof(pthread_t)*25))) //allocate memory space for threads
	{	
		perror ("Memory allocation error!!!");
		return (1);
		
	}
	
	//create a thread for every passenger
	for (i=0; i<25; i++){
		
		thrCheck = pthread_create( &pasThread[i], NULL, FOO , NULL);		
		if(thrCheck){
			fprintf(stderr,"Error - pthread_create() return code: %d\n",thrCheck);
			exit(EXIT_FAILURE);
		}
		
		
	}
	
	
	
	pthread_mutex_lock (&mtxEnd);
	
	
	
	
	
	return (0);
}


void synch_beg(){
	
	pthread_mutex_lock (&mtxMonitor);
	
	
}



void synch_end(){
	
	
	pthread_mutex_lock (&check);
	
	thosePassed--;
	printf ("those passed : %d\n", thosePassed);
	if (thosePassed==0){
		thosePassed=1;
		pthread_mutex_unlock (&mtxMonitor);
	}


	pthread_mutex_unlock (&check);
	
}


void synch_wait(){
	thosePassed++;
	countWait++;
	printf ("wait to pass%d\n", countWait);
	pthread_mutex_unlock (&mtxMonitor);
	
	pthread_mutex_lock (&mtxBusy);
	
}


void synch_notify(){
	
	if (countWait!=0){
		
		countWait--;
		pthread_mutex_unlock (&mtxBusy);
		
		
		
			
		countBlock++;
		pthread_mutex_lock (&mtxBlock);
			
		countBlock--;
			
	
	}
	
}

void synch_notifyAll(){
	
	if (countBlock!=0){
		pthread_mutex_unlock (&mtxBlock);
	}
	
	
	
}

void *FOO(){
	
	
	synch_beg();
	
	if (ranVar<9){
		ranVar++;
		synch_wait();
		
	}else{
		ranVar++;
	}
	
	
	ranVar--;
	
	printf ("Now it's %d\n", ranVar);
	
	
	synch_notify();
	synch_notifyAll();
	
	synch_end();
	
	return (NULL);
}





