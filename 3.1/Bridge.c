#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>


#define StringSize 100		//Maximum Car Number
#define Capacity 5		//Bridge Capacity

//Actions which is a car must do upon entering and exiting Bridge
void EnterCar(char *CarDir);
void ExitCar (char *CarDir);

//Car function
void * Car(void* carDir);

char currDir; // show current Direction of cars

pthread_mutex_t mtxEnter, mtxExit, mtxend; 
//mtxEnter-> Monitors EnterCar
//mtxExit-> Monitors ExitCar
//mtxend-> signals to end program when necessary
pthread_cond_t entBridge; //condition in Monitor Enter

int count1, count2;
//count1-> counts Cars that have entered Bridge
//count2-> counts Cars that have exited Bridge
int countl, countr;
// countl-> counts left cars waiting in queue
// countr-> counts right cars waiting in queue
int countlInc,countrInc;
//countlInc->counts left remaining cars to cross Bridge
//countrInc->counts right remaining cars to cross Bridge
int countlAll, countrAll; //They count all the left/right cars




int main (int argc, char *argv[]){
	char directions[StringSize]; //Sequence of incoming cars is stored here
	int i,thrCheck; 
	pthread_t *p_thrd;   // Used for thread creation
	
	//initialize variables
	count1=0; count2=Capacity;
	countl=0, countr=0;
	
	countrInc=0;
	countlInc=0;
	
	countrAll=0;
	countlAll=0;
	
	printf ("Please type the directions of the cars. l for left r for right:");
	scanf ("%s", directions);			
	printf ("\n");
	
	//initialize direction
	currDir= directions[0];
	
	//initialize counters
	for (i=0; i< strlen(directions); i++){
		if (directions[i]=='r'){
			countrInc++;
			countrAll++;                    
		}
		else if(directions[i]=='l'){		
			countlInc++;
			countlAll++;
		}
	}
	
	//initialize mutex, condition
	if (pthread_mutex_init(&mtxEnter, NULL) != 0)	
	{
		perror ("Mutex error");
		return 1;
	}
	
	
	if (pthread_mutex_init(&mtxExit, NULL) != 0)	
	{
		perror ("Mutex error");
		return 1;
	}
	
	if (pthread_mutex_init(&mtxend, NULL) != 0)	
	{
		perror ("Mutex error");
		return 1;
	}
	pthread_mutex_lock (&mtxend);
	
	
	if (pthread_cond_init(&entBridge, NULL) != 0)	
	{
		perror ("Condition error");
		return 1;
	}
	
	//Initialize threads
	if (NULL==(p_thrd= (pthread_t*)malloc(strlen(directions)*sizeof(pthread_t)))){	//dynamic space allocation for threads pointer
		perror ("Memory allocation error");
	}
	
	for (i=0; i< strlen(directions); i++){
		thrCheck = pthread_create( &p_thrd[i], NULL, Car , (void *)(&directions[i]));		
		if(thrCheck){
			fprintf(stderr,"Error - pthread_create() return code: %d\n",thrCheck);
			exit(EXIT_FAILURE);
		}
	}
	
	pthread_mutex_lock(&mtxend);		//wait till job is done
	
	printf ("No more incoming cars...\nTerminating Program...\n");
	
	//destroy every mutex.condition used
	pthread_mutex_destroy(&mtxend);
	pthread_mutex_destroy(&mtxEnter);
	pthread_mutex_destroy(&mtxExit);
	pthread_cond_destroy (&entBridge);
	//free allocated space
	free (p_thrd);				
	return 0;
}


void EnterCar(char *CarDir){
	int chFlag=0; //used to add to pointer countr/countl only one value per thread
	
	pthread_mutex_lock (&mtxEnter);		//Monitor Enter Starts Here
	
	//Decrease every time a car passes
	if (*CarDir=='r'){
		countrInc--;
		
	}
	if (*CarDir=='l'){
		countlInc--;
		
	}
	
	
	
	//Block cars which have no right to enter
	while ((*CarDir!=currDir)||(count1== Capacity)){
		
		
		//Count every car waiting
		if ((*CarDir=='r')&&(chFlag==0)){
			countr++;
			chFlag=1;
		}
		if ((*CarDir=='l')&&(chFlag==0)){
			countl++;
			chFlag=1;
		}
		
		pthread_cond_wait (&entBridge, &mtxEnter);
		//if the car signaled is not the appropriate wake up another one.
		if (currDir!=*CarDir){
			
			pthread_cond_signal (&entBridge);
		}
		
		
	}
	
	
	printf ("Enter %c direction\n", *CarDir);
	count1++;
	
	//If there are same direction cars waiting in queue awake them
	if ((currDir=='r')&&(countr!=0)&&(count1!=Capacity)){
		countr--;
		
		pthread_cond_signal (&entBridge);
		
	}else if ((currDir=='l')&&(countl!=0)&&(count1!=Capacity)){
		countl--;
		pthread_cond_signal (&entBridge);
		
	}
	
	
	
	pthread_mutex_unlock (&mtxEnter); //Monitor Enter Ends Here
	
}

void ExitCar(char *CarDir){
	
	pthread_mutex_lock (&mtxExit);		//Monitor Exit Starts Here 
	
	
	//Count how many cars exited the bridge
	count2--;
	printf ("Exit %c direction\n", *CarDir);
	
	if (*CarDir=='r'){
		countrAll--;
		
	}
	if (*CarDir=='l'){
		countlAll--;
		
	}
	
	
	
	//If there are no more cars from one side of the bridge
	// unlock the other side
	if ((countrAll==0)||(countlAll==0)){
		
		
		if ((countlAll)==0){
			
			currDir='r';
		}
		else if (countrAll==0){
			
			currDir='l';
		}
		//Always check if bridge is full and empty it
		if (count2==0){
			count1=0;
			count2=Capacity;}
			pthread_cond_signal (&entBridge);
	}
	//if there are cars from each direction coming
	//Every #capacity cars from one direction unlock the other 
	else if (count2==0){
		
		count2=Capacity;
		count1=0;
		//change direction
		if (currDir=='r'){
			currDir='l';
		}
		else if (currDir=='l'){
			currDir='r';
		}
		
		// if there are cars waiting in queue
		//and no more incoming currDirr cars 
		//wake up cars stuck in the queue
		if ((countrInc==0)&&(currDir=='r')){
			
			pthread_cond_signal (&entBridge);
		}
		if ((countlInc==0)&&(currDir=='l')){
			
			pthread_cond_signal (&entBridge);
		}
	}
	
	
	pthread_mutex_unlock (&mtxExit);	//Monitor Exit Starts Here 
}



void * Car(void* carDir){
	
	char * CarDir;
	CarDir =(char*)carDir;	
	//Car wants to Enter Bridge
	EnterCar(CarDir);
	
	
	//Car wants to Exit Bridge
	ExitCar(CarDir);
	
	
	//if there are no cars left Exit and terminate
	if ((countrAll + countlAll)==0){
		pthread_mutex_unlock (&mtxend);
	}
	
	return (carDir);
}
