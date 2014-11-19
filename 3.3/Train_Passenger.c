#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#define trainCap 10			//Train Capacity


void Train();

void * Passenger();

void synch_beg();

void synch_wait();

void synch_notify();

void synch_notifyAll();

void synch_end();

int timesToRun, passLeft;

int noBoard;

int capacity;

int thosePassed, countBlock, countWait;

pthread_mutex_t mtxBlock, mtxBusy, end, check, mtxMonitor;

int main (int argc, char* argv[]){
	int i, noPass, thrCheck; //noPass-> used to read the total number of passengers
	pthread_t *pasThread; // used int thread creation
	
	
	thosePassed=1;
	countBlock=0;
	countWait=0;


	
	//read input from user (#passengers)
	printf ("How many passengers? ");
	scanf (" %d", &noPass);				
	printf ("\n");
	
	//compute timesToRun and passLeft
	timesToRun= noPass/trainCap;			
	passLeft= noPass%trainCap;	
	
	
	//initialize counters noBoard, noRide and trainCap
	if (timesToRun!=0){
		capacity= trainCap;
		
	}
	// if there are not enough passengers (#passengers< capacity) change capacity to the appropriate number
	else{
		capacity= passLeft;
		
		
	}
	noBoard=0;
	
	
	//initialize mutexes
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
	
	
	
	//initialize threads
	if (NULL==(pasThread= (pthread_t*)malloc(sizeof(pthread_t)*noPass))) //allocate memory space for threads
	{	
		perror ("Memory allocation error!!!");
		return (1);
		
	}
	
	//create a thread for every passenger
	for (i=0; i<noPass; i++){
		
		thrCheck = pthread_create( &pasThread[i], NULL, Passenger , NULL);		
		if(thrCheck){
			fprintf(stderr,"Error - pthread_create() return code: %d\n",thrCheck);
			exit(EXIT_FAILURE);
		}
		
		
	}
	
	
	
	for (i=0; i<noPass; i++){
		pthread_join(pasThread[i], NULL);
	} // wait till there are no more passengers
	
	
	
	printf ("WHAATT?!?!? This place is empty :(\n");
	printf ("I'll be better going home\n");
	//destroy mutexes/conditions

	pthread_mutex_destroy (&mtxMonitor);
	pthread_mutex_destroy (&mtxBlock);
	pthread_mutex_destroy (&mtxBusy);
	pthread_mutex_destroy (&check);
	
	free (pasThread);		//free allocated space
	return (0);
	
}



void synch_beg(){
	
	pthread_mutex_lock (&mtxMonitor);
	
	
}



void synch_end(){
	
	
	pthread_mutex_lock (&check);
	
	thosePassed--;
	
	if (thosePassed==0){
		thosePassed=1;
		pthread_mutex_unlock (&mtxMonitor);
	}


	pthread_mutex_unlock (&check);
	
}


void synch_wait(){
	thosePassed++;
	countWait++;
	
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



void  Train(){


	
	printf ("YOOHOO\n");
		
	
	
}

void *Passenger(){
	static int flag=0;
	int i;
	
	synch_beg();
	
	if (noBoard!=(capacity-1)){
		
		
		noBoard++;
		synch_wait();
		
	}
	
	else {noBoard++;}
	
	
	if (flag==0){
		timesToRun--;
		printf ("Startin Train!!!!\n");
		flag=1;
		if ((timesToRun==0)&&(passLeft!=0)){
			capacity=passLeft;
			passLeft=0;
			
		}
	
	}
	
	Train();	
		
		
	synch_notify();
	
	printf ("BOO HOO HOOOOOO ride is over\n");
	
	synch_notifyAll();
	noBoard--;
	if (noBoard==0){
		flag=0;
		printf ("Returning back\n");
		for (i=0; i<3; i++){
			sleep (1);
			printf ("...\n");
		}
	}
	synch_end();
	
	return (NULL);
	
}



