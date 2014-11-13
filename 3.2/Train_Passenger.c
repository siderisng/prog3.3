#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#define trainCap 10			//Train Capacity

//This complex function is used to simulate the passengers reaction when he/she rides the train
void Train();
//Passenger's actions when he wants to board on train
void * Passenger();


int timesToRun, passLeft;
//timesToRun-> given the number of passengers, the number of rides to serve them all
//passLeft-> how many passengers will board the train in the last ride
int noBoard, noRide;
//noBoard-> #passengers waiting to ride train
//noRide-> #passenger riding Train
int rideFlag, endflag;
//rideFlag-> used to block cars when appropriate
//endflag-> used to signal the final ride
int capacity;//capacity of train


pthread_mutex_t entry, end;
// mutex entry used for monitor Entry
pthread_cond_t toBoard, toStart;
//toBoard-> wait to assemble appropriate number of passengers or wait current ride to end
//toStart-> wait for #capacity passengers to ride train 



int main (int argc, char* argv[]){
	int i, noPass, thrCheck; //noPass-> used to read the total number of passengers
	pthread_t *pasThread; // used int thread creation
	
	//initialize flags
	endflag=0;
	rideFlag=1;
	
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
		noRide=capacity;
	}
	// if there are not enough passengers (#passengers< capacity) change capacity to the appropriate number
	else{
		capacity= passLeft;
		noRide=passLeft;
		endflag=1;
	}
	noBoard=0;
	
	
	//initialize mutexes
	if (pthread_mutex_init(&entry, NULL) != 0)	
	{
		perror ("Mutex error");
		return 1;
	}
	
	if (pthread_mutex_init(&end, NULL) != 0)	
	{
		perror ("Mutex error");
		return 1;
	}
	pthread_mutex_lock (&end);
	
	//initialize conditions
	if (pthread_cond_init(&toBoard, NULL) != 0)
	{
		perror ("Condition error");
		return 1;
	}

	
	if (pthread_cond_init(&toStart, NULL) != 0)
	{
		perror ("Condition error");
		return 1;
	}
	
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
	
	
	
	pthread_mutex_lock (&end); // wait till there are no more passengers
	
	printf ("WHAATT?!?!? This place is empty :(\n");
	printf ("I'll be better going home\n");
	//destroy mutexes/conditions
	pthread_mutex_destroy (&entry);
	pthread_mutex_destroy (&end);
	pthread_cond_destroy (&toStart);
	pthread_cond_destroy (&toBoard);
	
	free (pasThread);		//free allocated space
	return (0);
	
}


void  Train(){


	
	printf ("YOOHOO\n");
		
	
	
}

void *Passenger(){
	int i;
	
	pthread_mutex_lock (&entry);		//Start of Monitor Entry
	
	//add one to noBoard everytime someone is waiting to ride the train
	noBoard ++;
	
	//if there are no enough passengers or when train is active wait
	if ((noBoard<capacity)||(rideFlag==0)){				
		pthread_cond_wait (&toBoard, &entry );
		//decrease noBoard when someone enters train
		noBoard--;
	}

	//decrease noBoard for the first to ride it
	if (rideFlag==1){
	
		noBoard--;
		
		rideFlag=0;
	}
	
	//if all the passengers are here start train
	if (noRide==capacity){
		
		printf ("Starting Train\n");
		timesToRun--;
	}
	Train ();
	//decrease when someone enters train
	noRide--;
	
	//wait till ride is over(all the passengers have called Train())
	if (noRide!=0){
		pthread_cond_signal (&toBoard);
		pthread_cond_wait (&toStart, &entry);
	}
	//when ride is over initialize counters
	else {
		noRide=capacity;
	}
	
	printf ("Leaving Train... So Sad\n");
	noRide--;
	
	//signal everyone to exit train
	pthread_cond_signal (&toStart);
	
	//when everyone exits return train
	if (noRide==0){
		printf ("Returning Back\n");
			for (i=0; i<3; i++){
				sleep (1);
				printf (".\n");
			}
		
		//if there are no more passengers terminate program
		if ((timesToRun==0)&&(noRide==0)&&(passLeft==0)){
			
			pthread_mutex_unlock (&end);
			
		}//if there are less passengers than the train capacity remaining, change capacity
		else if ((timesToRun==0)&&(passLeft!=0)){
			capacity = passLeft;
			endflag=1;
		}//if all of those passengers have ride the train end ride
		else if (endflag==1){
			pthread_mutex_unlock (&end);
		}
		//initialize noRide
		noRide=capacity;
		
		//if there are enough passengers waiting in queue wake them up
		if (noBoard>=capacity){
			pthread_cond_signal (&toBoard);
		}//else get new ones
		else {
			rideFlag=1;
		}
		
	}
	
	
	pthread_mutex_unlock (&entry); //End of Monitor Entry
	
	return (NULL);
	
}



