#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>

#define StringSize 100		//megistos arithmos amaksiwn synolika
#define Capacity 5		//xwritikotita gefyras

void EnterCar(char *CarDir);
void ExitCar (char *CarDir);

void * Car(void* carDir);

char currDir;
pthread_mutex_t mtxEnter, mtxExit, mtxend;
pthread_cond_t entBridge;
int count1, count2,countl, countr, flag, count, countlall,countrall, countl2, countr2;
int carsPassed, carsleft, flag,waiting,startflag;


int main (int argc, char *argv[]){
 	char directions[StringSize];
	int i,thrCheck;
	pthread_t *p_thrd;
	
	
	
	currDir='r';
	
	flag=0;
	startflag=0;
	count1=0; count2=Capacity;
	countl=0, countr=0;
	carsPassed= Capacity;
	carsleft=0;
	printf ("Please type the directions of the cars. l for left r for right:");
	scanf ("%s", directions);			//pairnoume eisodo gia ta aytokinita (l gia aristera, r gia deksia)
	printf ("\n");
	count=strlen(directions);
	countrall=0;
	countlall=0;
	countr2=0;
	countl2=0;
	
	for (i=0; i< strlen(directions); i++){
		if (directions[i]=='r'){
			countrall++;
			countr2++;//afksanoume to countr gia kathe amaksi apo deksia
		}
		else if(directions[i]=='l'){		//afksanoume to countl gia kathe amaksi apo aristera
			countlall++;
			countl2++;
		}
	}
	
	if (pthread_mutex_init(&mtxExit, NULL) != 0)	//arxikopoihsh mutex mtxExit
    {
        perror ("Mutex error");
        return 1;
    }
	
	if (pthread_cond_init(&entBridge, NULL) != 0)	//arxikopoihsh mutex mtxExit
    {
        perror ("Condition error");
        return 1;
    }
	
	
	
	if (pthread_mutex_init(&mtxend, NULL) != 0)	//arxikopoihsh mutex mtxend kai kleidwma
    {
        perror ("Mutex error");
        return 1;
    }
	
	pthread_mutex_lock (&mtxend);

	
	if (NULL==(p_thrd= (pthread_t*)malloc(strlen(directions)*sizeof(pthread_t)))){	//desmefsi xwrou gia ton pinaka apo threads
		perror ("Memory allocation error");
	}
		
	for (i=0; i< strlen(directions); i++){
		thrCheck = pthread_create( &p_thrd[i], NULL, Car , (void *)(&directions[i]));		//dimiourgia threads
		if(thrCheck){
			fprintf(stderr,"Error - pthread_create() return code: %d\n",thrCheck);
			exit(EXIT_FAILURE);
		}
	}
	pthread_mutex_lock(&mtxend);		//lockarei edw mexri na oloklirwthei oli i diadikasia
	
	
	
	pthread_mutex_destroy(&mtxend);	//diagrafi mutexes
	pthread_mutex_destroy(&mtxEnter);
	pthread_mutex_destroy(&mtxExit);
	free (p_thrd);				//apeleftherwsi xwrou gia ton pinaka apo threads
	return 0;
}


void EnterCar(char *CarDir){
	int chFlag=0;
	
	pthread_mutex_lock (&mtxEnter);		//mono ena thread bainei kathe fora sto krisimo simeio 
		if (*CarDir=='r'){
			countrall--;
			
		}
		if (*CarDir=='l'){
			countlall--;
			
		}
	while ((*CarDir!=currDir)||(count1== Capacity)){
		
		
		
		if ((*CarDir=='r')&&(chFlag==0)){
			countr++;
			chFlag=1;
		}
		if ((*CarDir=='l')&&(chFlag==0)){
			countl++;
			chFlag=1;
		}
		pthread_cond_wait (&entBridge, &mtxEnter);
		if (currDir!=*CarDir){
			
			pthread_cond_signal (&entBridge);
		}
		
		
	}
	
	
	if (*CarDir=='r'){
			countr2--;
			
		}
	if (*CarDir=='l'){
			countl2--;
			
		}
	
	
	printf ("Enter %c direction\n", *CarDir);
	count1++;
	if ((currDir=='r')&&(countr!=0)&&(count1!=Capacity)){
		countr--;
			
		pthread_cond_signal (&entBridge);
	
	}else if ((currDir=='l')&&(countl!=0)&&(count1!=Capacity)){
		countl--;
		pthread_cond_signal (&entBridge);
		
	}
	
	
	
	pthread_mutex_unlock (&mtxEnter);
	
}

void ExitCar(char *CarDir){
	pthread_mutex_lock (&mtxExit);		//mono ena thread bainei kathe fora sto krisimo simeio 
	
	
	
	count2--;
	printf ("Exit %c direction\n", *CarDir);
	
	if ((countr2==0)||(countl2==0)){
		
		
		if ((countl2)==0){
			currDir='r';
		}
		else if (countr2==0){
			currDir='l';
		}
		if (count2==0){
			printf ("\n\n");
			count1=0;
			count2=Capacity;}
		pthread_cond_signal (&entBridge);
	}
	else if (count2==0){
		
		printf ("\n\n");
		
		count2=Capacity;
		count1=0;
		if (currDir=='r'){
			currDir='l';
		}
		else if (currDir=='l'){
			currDir='r';
		}
		printf ("%d\n", countrall);
		
		if ((countrall==0)&&(currDir=='r')){
			flag=1;
			pthread_cond_signal (&entBridge);
		}
		if ((countlall==0)&&(currDir=='l')){
			flag=1;
			pthread_cond_signal (&entBridge);
		}
	}

	
	
	pthread_mutex_unlock (&mtxExit);		//kseblockarisma gia to epomeno thread 
}



void * Car(void* carDir){
	
	char * CarDir;
	CarDir =(char*)carDir;	

	EnterCar(CarDir);
	
	
	ExitCar(CarDir);
	
	
	return (carDir);
}
