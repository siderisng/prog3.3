#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>


#define StringSize 100		//Maximum Car Number
#define Capacity 5		//Bridge Capacity

void synch_beg();

void synch_wait(char *CarDir);

void synch_notify(char *CarDir);

void synch_notifyAll();

void synch_end();

void full_beg();

void full_end();

void full_notify();
//Actions which is a car must do upon entering and exiting Bridge
void EnterCar(char *CarDir);
void ExitCar (char *CarDir);

//Car function
void * Car(void* carDir);

char currDir; // show current Direction of cars

pthread_mutex_t mtxEnter, mtxExit, mtxend, mtxWait; 
//mtxEnter-> Monitors EnterCar
//mtxExit-> Monitors ExitCar
//mtxend-> signals to end program when necessary

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
	
	if (pthread_mutex_init(&mtxWait, NULL) != 0)	
	{
		perror ("Mutex error");
		return 1;
	}
	pthread_mutex_lock (&mtxWait);
	
	

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
	//free allocated space
	free (p_thrd);				
	return 0;
}





void * Car(void* carDir){
	int chFlag=0;
	char * CarDir;
	CarDir =(char*)carDir;	
	
	synch_beg();
	
	
	//Car wants to Enter Bridge
		if (*CarDir=='r'){
		countrInc--;
		
	}
	if (*CarDir=='l'){
		countlInc--;
		
	}
	
		while ((*CarDir!=currDir)||(count1== Capacity)){
		  if ((*CarDir=='r')&&(chFlag==0)){
		    countr++;
		   //chFlag=1;
		  }
		  if ((*CarDir=='l')&&(chFlag==0)){
		    countl++;
		  // chFlag=1;
		  }
		  
		synch_wait(CarDir);

		//if the car signaled is not the appropriate wake up another one.
		if (currDir!=*CarDir){
			
			//synch_wait(CarDir);
			synch_notify(CarDir);
		}
		synch_notify(CarDir);
	}
	

	count1++;
	printf ("Enter %c direction\n", *CarDir);
	
	if ((currDir=='r')&&(countr!=0)&&(count1!=Capacity)){
	  countr--;
	  
	  synch_notify(CarDir);
	  
	}else if ((currDir=='l')&&(countl!=0)&&(count1!=Capacity)){
	  countl--;
	 synch_notify(CarDir);
	  
	}
	
	synch_end();

	full_beg();
	
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
		
			count1=0;
			count2=Capacity;
		  
		
		full_notify();
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
			
			full_notify();
		}
		if ((countlInc==0)&&(currDir=='l')){
			
			full_notify();
		}
	}
	

	full_end();
	
	//printf("lefts waiting:%d,rights waiting:%d\n",countl,countr);
	
	//if there are no cars left Exit and terminate
	if ((countrAll + countlAll)==0){
		pthread_mutex_unlock (&mtxend);
	}
	
	return (carDir);
}

void full_beg(){
  pthread_mutex_lock (&mtxExit);	
  
}

void full_end(){
  pthread_mutex_unlock (&mtxExit);	
}

void full_notify(){
  pthread_mutex_unlock(&mtxEnter);
}

void synch_notify(char *CarDir){
	


	pthread_mutex_unlock(&mtxWait);
}

void synch_wait(char *CarDir){

 
  //printf("%c car waiting\n",*CarDir);
	pthread_mutex_unlock (&mtxEnter);
	
	pthread_mutex_lock (&mtxWait);
	
  if (*CarDir=='r'){
    countr--;
  }
  if (*CarDir=='l'){
    countl--;
  }
	//printf (" %c car woke up\n", *CarDir);

}

void synch_beg(){
	
	pthread_mutex_lock (&mtxEnter);
	
	
}

void synch_end(){
	
	
	pthread_mutex_unlock (&mtxEnter); //Monitor Enter Ends Here

	
}
