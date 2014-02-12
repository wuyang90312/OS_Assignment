/**
* Purpose: Solve assignment3 -- Spooling printer(synchronization)
* Author: Yang Wu -- yang.wu2@mail.mcgill.ca
* Date: Feb 11th 2014*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define EXIT_SUCCESS 0 

void* ClientFunc()
{
	printf("The output is added\n");
}

int main()
{
	pthread_t client[5]; 	/* define 5 clients */
	pthread_t printer[2];	/* define 2 clients */
	int sign;	/* show if the thread creation is success or not */
	
	/* create two while loops: one for client, the other for printer */
	int count;
	for(count = 0; count < 5; count++)
	{
		sign = pthread_create(&client[count], NULL, ClientFunc, NULL);
		if(sign != 0) /* once the creation is fail, return no zero number, print out the failure */
			exit(sign);
			
		sign = pthread_join(client[count], NULL);
		if(sign != 0)
			exit(sign);
	}
	
	for(count = 0; count < 2; count++)
	{
		printf("hello, enter the loop %d", count);
	}

	return EXIT_SUCCESS; /* once everything work fine, exit with return 0*/
}
