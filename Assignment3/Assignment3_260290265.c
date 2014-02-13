/**
* Purpose: Solve assignment3 -- Spooling printer(synchronization)
* Author: Yang Wu -- yang.wu2@mail.mcgill.ca
* Date: Feb 11th 2014*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define EXIT_SUCCESS 0

static int file[7]; /* memory buffer */
static int index;

int monitor() /* monitor session: used for synchronization*/
{
	
}

void* ClientFunc(void* args) /* once the function is called, start a new printing assignment */
{
	char* s = (char*) args;
	int random = rand()%10 + 1; /* generate a random number from 1 to 10*/
	
	file[index] = random;
	index++; /* when increment count, add a mutex */
	
	printf("The output -%s- is %d : %d\n ", s, index-1, file[index-1]);

}

void* PrintFunc(void* args) /* once the function is called, start the printing task */
{
	sleep(file[index]);
	printf("The printer is %d: %d\n", index, file[index]);
	index++;
}

int main()
{
	pthread_t client[5]; 	/* define 5 clients */
	pthread_t printer[2];	/* define 2 clients */
	pthread_attr_t attr;
	int sign;	/* show if the thread creation is success or not */
	
	index = 0; /* Initialize the index for thread */
	
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
		printf("the number is %d", count);
		char tmp[1];
		sprintf(tmp, "%d", count);
		sign = pthread_create(&client[count], &attr, ClientFunc, tmp); /* create thread for client */
		if(sign != 0) /* once the creation is fail, return no zero number, print out the failure */
			exit(sign);
			
		/*sign = pthread_join(client[count], NULL); /* Joint all of the thread together*/
		/*if(sign != 0)
			exit(sign);*/
	}
	index = 0;
	for(count = 0; count < 2; count++)
	{
		sign = pthread_create(&printer[count], &attr, PrintFunc, NULL); /* create thread for client */
		if(sign != 0) /* once the creation is fail, return no zero number, print out the failure */
			exit(sign);
			
	}
	sleep(10);
	printf("finish successfully\n");

	return EXIT_SUCCESS; /* once everything work fine, exit with return 0*/
}
