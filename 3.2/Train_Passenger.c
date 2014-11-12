#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#define trainCap 10			//xwritikotita trainou

void Train();

void * Passenger();


int timesToRun, passLeft,noBoard, noRide, rideFlag, capacity, flag, startFlag, endflag;
pthread_mutex_t entry, ride, end;
pthread_cond_t toBoard, toStart;



int main (int argc, char* argv[]){
	int i, noPass, thrCheck;
	pthread_t *pasThread;
	
	endflag=0;
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
	
	printf ("WHAATT?!?!? This place is empty :(\n");
	printf ("I'll be better going home\n");
	//diagrafi twn mutexes
	pthread_mutex_destroy (&entry);
	pthread_mutex_destroy (&ride);
	pthread_mutex_destroy (&end);
	pthread_cond_destroy (&toStart);
	pthread_cond_destroy (&toBoard);
	
	free (pasThread);		//apeleftherwsi xwrou tou pinaka twn threads
	return (0);
	
}


void  Train(){


	
	printf ("YOOHOO\n");
		
	
	
}

void *Passenger(){
	int i;
	
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
		
		printf ("Starting Train\n");
		timesToRun--;
	}
	Train ();
	noRide--;
	
	if (noRide!=0){
		pthread_cond_signal (&toBoard);
		pthread_cond_wait (&toStart, &entry);
	}
	else {
		noRide=capacity;
	}
	
	pthread_cond_signal (&toStart);
	printf ("Leaving Train... So Sad\n");
	noRide--;
	
	if (noRide==0){
		printf ("Returning Back\n");
			for (i=0; i<3; i++){
				sleep (1);
				printf (".\n");
			}
		
		
		if ((timesToRun==0)&&(noRide==0)&&(passLeft==0)){
			
			pthread_mutex_unlock (&end);
			
		}else if ((timesToRun==0)&&(passLeft!=0)){
			capacity = passLeft;
			endflag=1;
		}else if (endflag==1){
			pthread_mutex_unlock (&end);
		}
		noRide=capacity;
		
		if (noBoard>=capacity){
			pthread_cond_signal (&toBoard);
		}else {
			rideFlag=1;
		}
		
	}
	
	
	pthread_mutex_unlock (&entry);
	
	return (NULL);
	
}



