/**
* Purpose: Solve assignment3 -- Spooling printer(synchronization)
* Author: Yang Wu -- yang.wu2@mail.mcgill.ca
* Date: Feb 11th 2014*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define EXIT_SUCCESS 0
#define CLIENT_CLASS 0
#define PRINTER_CLASS 1

static int file[7]; /* memory buffer */
static int index, indexP, Process;
sem_t mutex, Empty, Full; /* declare the semaphore as global variables*/

int monitor(int class, int* ID) /* monitor session: used for synchronization*/
{
	
	if(class == CLIENT_CLASS) /* if clients enter the monitor */
	{
		sem_wait(&mutex); /* lock the critical session */
		index++; /* client index */
		*ID = index%7;
		sem_post(&mutex); /* unlock the critical session */
		
		sem_wait(&Empty);	/*First check if there is empty space*/
							/*If there is, keep going; wait otherwise*/
	}else if(class == PRINTER_CLASS) /* if printers enter the monitor */
	{
		sem_wait(&mutex); 
		indexP++; /* printer index */
		*ID = indexP%7;
		sem_post(&mutex);
		
		sem_wait(&Full);	/*First check if there is loaded data*/
							/*If there is, keep going; wait otherwise*/
	}else /* unknown cases*/
	{
		printf("Error: Unknown Class Identification!!!!");
		exit(EXIT_FAILURE);
	}

	return 0;
}

void* ClientFunc(void* args) /* once the function is called, start a new printing assignment */
{
	int i;
	for(i = 0; i<3; i++){
		int ID;
		monitor(CLIENT_CLASS, &ID);
		int random = rand()%10 + 1; /* generate a random number from 1 to 10*/
		
		file[ID] = random;

		printf("The client%s stores %d to queue %d\n", (char*) args, file[ID], ID);
		sem_post(&Full); /* Once a data is stored, signal full to increment*/
	}
	
	Process++;/*Once client finished upload, signal the program*/
	pthread_exit(0); /* exit thread */

}

void* PrintFunc(void* args) /* once the function is called, start the printing task */
{
	int ID;
	while(Process != 5 || index != indexP)
	{
		monitor(PRINTER_CLASS, &ID);
		printf("The printer%s  prints %d frome queue %d\n", (char*)args, file[ID], ID);
		sleep(file[ID]);
		sem_post(&Empty); /* Once a data is printed, signal empty to increment*/
	}
	pthread_exit(0); /* exit thread */
}

int main()
{
	pthread_t client[5]; 	/* define 5 clients */
	pthread_t printer[2];	/* define 2 clients */
	pthread_attr_t attr;

	int sign;	/* show if the thread creation is success or not */
	
	sem_init(&mutex, 0, 1);		/* initialize mutex to 1 - binary semaphore */
								/* second param = 0 - semaphore is local */
	sem_init(&Empty, 0, 7);
	sem_init(&Full, 0, 0);
	
	index = -1; /* Initialize the index for thread */
	indexP = -1;
	Process = 0;
	
	sign = pthread_attr_init(&attr); /* Intialize the thread attribute*/
	if(sign != 0)
		exit(sign);
	
	sign = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if(sign != 0)
		exit(sign);
	
	/* create client threads */

	sign = pthread_create(&client[0], &attr, ClientFunc, "1"); /* create thread for client */
	if(sign != 0) /* once the creation is fail, return no zero number, print out the failure */
		exit(sign);
	sign = pthread_create(&client[1], &attr, ClientFunc, "2"); /* create thread for client */
	if(sign != 0) /* once the creation is fail, return no zero number, print out the failure */
		exit(sign);
	sign = pthread_create(&client[2], &attr, ClientFunc, "3"); /* create thread for client */
	if(sign != 0) /* once the creation is fail, return no zero number, print out the failure */
		exit(sign);
	sign = pthread_create(&client[3], &attr, ClientFunc, "4"); /* create thread for client */
	if(sign != 0) /* once the creation is fail, return no zero number, print out the failure */
		exit(sign);
	sign = pthread_create(&client[4], &attr, ClientFunc, "5"); /* create thread for client */
	if(sign != 0) /* once the creation is fail, return no zero number, print out the failure */
		exit(sign);

	/* create printer threads */

	sign = pthread_create(&printer[0], &attr, PrintFunc, "1"); /* create thread for client */
	if(sign != 0) /* once the creation is fail, return no zero number, print out the failure */
		exit(sign);
		
	sign = pthread_create(&printer[1], &attr, PrintFunc, "2"); /* create thread for client */
	if(sign != 0) /* once the creation is fail, return no zero number, print out the failure */
		exit(sign);

	while(Process != 5 || index != indexP)
		sleep(1);
	printf("Finish successfully\n");

	sem_destroy(&mutex); /* destroy semaphore */
	sem_destroy(&Empty);
	sem_destroy(&Full);
	
	return EXIT_SUCCESS; /* once everything work fine, exit with return 0*/
}
