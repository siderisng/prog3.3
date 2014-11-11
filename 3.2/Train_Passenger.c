#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#define trainCap 10			//xwritikotita trainou

void Train();

void * Passenger();


int timesToRun, passLeft,noBoard, noRide, rideFlag, capacity, flag, startFlag;
pthread_mutex_t entry, ride, end;
pthread_cond_t toBoard, toStart;



int main (int argc, char* argv[]){
	int i, noPass, thrCheck;
	pthread_t *pasThread;
	
	startFlag =1;
	noBoard=0;
	flag=0;
	printf ("How many passengers? ");
	scanf (" %d", &noPass);				//dilwnoume posoi einai oi synolikoi epivates
	printf ("\n");
	
	timesToRun= noPass/trainCap;			//poses fores tha ksekinisei to treno (o pragmatikos arithmos einai timesToRun+1)
	passLeft= noPass%trainCap;			//posoi epivates perisevoun sto teleytaio treno
	capacity= trainCap;				//xwritikotita trenou
	noRide=capacity;
	rideFlag=1;
	if (pthread_mutex_init(&end, NULL) != 0)	//arxikopoihsh mutex end kai kleidwma
	{
		perror ("Mutex error");
		return 1;
	}
	pthread_mutex_lock (&end);
	
	if (pthread_mutex_init(&ride, NULL) != 0)	//arxikopoihsh mutex end kai kleidwma
	{
		perror ("Mutex error");
		return 1;
	}
	
	if (pthread_mutex_init(&entry, NULL) != 0)	//arxikopoihsh mutex entry
	{
		perror ("Mutex error");
		return 1;
	}
	
	
	if (pthread_cond_init(&toBoard, NULL) != 0)	//arxikopoihsh mutex board kai kleidwma
	{
		perror ("Condition error");
		return 1;
	}

	
	if (pthread_cond_init(&toStart, NULL) != 0)	//arxikopoihsh mutex board kai kleidwma
	{
		perror ("Condition error");
		return 1;
	}
	
	if (NULL==(pasThread= (pthread_t*)malloc(sizeof(pthread_t)*(noPass+1)))){	//desmefsi xwrou gia ton pinaka apo threads
		perror ("Memory allocation error!!!");
		return (1);
		
	}
	
	
	for (i=0; i<noPass; i++){
		
		thrCheck = pthread_create( &pasThread[i], NULL, Passenger , NULL);		//dimiourgia threads
		if(thrCheck){
			fprintf(stderr,"Error - pthread_create() return code: %d\n",thrCheck);
			exit(EXIT_FAILURE);
		}
		
		
	}
	
	
	
	pthread_mutex_lock (&end);	//blokarisma mexri na oloklirwthei i diadikasia
		//diagrafi twn mutexes
	pthread_mutex_destroy (&entry);
	pthread_mutex_destroy (&ride);
	
	
	free (pasThread);		//apeleftherwsi xwrou tou pinaka twn threads
	return (0);
	
}


void  Train(){
	int i;

		if (flag==1){
			printf ("Returning back\n");
			for (i=0; i<3; i++){
				sleep (1);
				printf (".\n");
			}
			flag=0;
		}
	
		printf ("Starting Ride \n" );
		
	
	
}

void *Passenger(){
	
	
	pthread_mutex_lock (&entry);		//blokarisma twn epivatwn mexri na vevaiwthoume oti xwrane
	
	noBoard ++;
	
	if ((noBoard<=capacity)||(rideFlag==0)){				
		pthread_cond_wait (&toBoard, &entry );
		noBoard--;
	}

	
	if (rideFlag==1){
	
		noBoard--;
		
		rideFlag=0;
	}
	
	
	if (noRide==capacity){
		
		Train();
		
	}
	noRide--;
	
	
	
	printf ("Leaving Train... So Sad\n");
	
	
	if (noRide==0){
		flag=1;
		noRide=capacity;
		
		if (noBoard>=capacity){
			pthread_cond_signal (&toBoard);
		}else {
			rideFlag=1;
		}
		
	}
	printf ("%d\n", noRide);
	if (noRide!=capacity){
		pthread_cond_signal (&toBoard);
	}
	printf ("%d\n", noBoard);
	pthread_mutex_unlock (&entry);
	
	return (NULL);
	
}



