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

static int file[7]; /* memory buffer */
static int index, indexP;
sem_t mutex, memLock;

int monitor(int ID) /* monitor session: used for synchronization*/
{
	sem_wait(&mutex);  /* lock the critical session */
	ID++; /* when increment count, add a mutex */
	sem_post(&mutex); /* unlock the critical session */
	return ID;
}

void* ClientFunc(void* args) /* once the function is called, start a new printing assignment */
{
	char s[5];
	strcpy(s, (char*)args);
	sem_post(&memLock); /* once the string is copied, release the memory*/
	index = monitor(index);
	int random = rand()%10 + 1; /* generate a random number from 1 to 10*/
	
	file[index-1] = random;

	
	printf("The output -%s- is %d : %d\n ", s, index-1, file[index-1]);
	
	pthread_exit(0); /* exit thread */

}

void* PrintFunc(void* args) /* once the function is called, start the printing task */
{
	char s[5];
	strcpy(s, (char*)args);
	sem_post(&memLock); /* once the string is copied, release the memory*/
printf("enter\n");
	int ID = monitor(indexP);
	sleep(file[indexP]);
	printf("The printer is %s: %d\n", s, file[ID-1]);

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
	sem_init(&memLock, 0, 1);
	printf("The mutex is %d\n", mutex);
	index = 0; /* Initialize the index for thread */
	indexP = 0;
	
	sign = pthread_attr_init(&attr); /* Intialize the thread attribute*/
	if(sign != 0)
		exit(sign);
	
	sign = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if(sign != 0)
		exit(sign);
	
	/* create two while loops: one for client, the other for printer */
	int count;
	for(count = 0; count < 5; count++)
	{
		printf("the number is %d and the memlock is %d\n", count, memLock);
		char tmp[5];
		sem_wait(&memLock); /* Make sure that only one thread access shared memory each time */
		sprintf(tmp, "%d", count);
		
		sign = pthread_create(&client[count], &attr, ClientFunc, tmp); /* create thread for client */
		if(sign != 0) /* once the creation is fail, return no zero number, print out the failure */
			exit(sign);

		/*sign = pthread_join(client[count], NULL); /* Joint all of the thread together*/
		/*if(sign != 0)
			exit(sign);*/
	}


	for(count = 0; count < 2; count++)
	{
		printf("2. the number is %d and the memlock is %d, the index is %d\n", count, memLock, index);
		char tmp[5];
		sem_wait(&memLock); /* Make sure that only one thread access shared memory each time */
		sprintf(tmp, "%d", count);

		sign = pthread_create(&printer[count], &attr, PrintFunc, tmp); /* create thread for client */
		if(sign != 0) /* once the creation is fail, return no zero number, print out the failure */
			exit(sign);
			
	}
	sleep(20);
	printf("finish successfully\n");

	sem_destroy(&memLock); /* destroy semaphore */
	sem_destroy(&mutex);
	
	return EXIT_SUCCESS; /* once everything work fine, exit with return 0*/
}
