/**
* Purpose: Solve assignment 4 -- Deadlock Avoidance/Detection
* Author: Yang Wu -- yang.wu2@mail.mcgill.ca
* Date: March 17th 2014*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define AVOIDANCE	"avoidance"
#define DETECTION	"detection"

static int threadNum, resourceType;
static int *globalCond;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;	

/*
 *********************Declaration of all the classes********************/
int Banker(int,int);
void* threadCreation(void*);
int allocateResources(int, int*);
 /*
  ********************End of the Declaration****************************/

int main(int arg, char* argv[]) /* Todo: to finishe the assignment, add more types*/
{
	int sign, loop, tmp;
	pthread_attr_t attr;
		
/******************Check the states of arguments & Initialize the global variables************/	
	if(arg < 5) /*If the argument is less than 4, the argument is not enough*/
	{
		fprintf(stderr, "ERROR: Lack of Arguments!!!\n");
		return EXIT_FAILURE;
	}
	
	resourceType = arg-4; /* Intialize the condition variables*/
	globalCond = malloc(resourceType*sizeof(int));
	for(loop = 3; loop < arg-1;  loop++)
	{
		globalCond[loop-3] = atoi(argv[loop]); /* Allocate different resources */
		if(!globalCond[loop-3])
		{
			fprintf(stderr, "ERROR: Invalid of Argument: \nArgument %d is supposed to be a positive integer\n", loop);
			return EXIT_FAILURE;
		}
	}

/*************************Initialize the global variables********************************/
	threadNum = atoi(argv[2]);
	if(!threadNum)
	{
		fprintf(stderr, "ERROR: Invalid of Argument: \nArgument 2 is supposed to be a positive integer\n");
		return EXIT_FAILURE;
	}

	/*Create a pthread according to the set number*/
	pthread_t thread[threadNum];

/***********************Decide the type of resource allocator****************************/
	if(strcasecmp(DETECTION,argv[1]) && strcasecmp(AVOIDANCE,argv[1]))
	{
		fprintf(stderr, "ERROR: Invaid Type of Resource Allocator!!!\n");
		return EXIT_FAILURE;
	}
	
/***********************Create a thread with the set amount****************************/
	for(loop = 0; loop < threadNum; loop++)
	{
		sign = pthread_attr_init(&attr); /* Intialize the thread attribute*/
		if(sign != 0)
			exit(sign);

		sign = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		if(sign != 0)
			exit(sign);
			
		sign = pthread_create(&thread[0], &attr, threadCreation, argv[1]); /* create thread for client */
		if(sign != 0) /* once the creation is fail, return no zero number, print out the failure */
			exit(sign);
	}
	printf("2 succuess\n"); //todo: clean up the sentence
	sleep(2);
	return EXIT_SUCCESS;
}

int Banker() /*The whole section is set as a critical session so that only one process can access the Banker at the same time*/
{
	rc = pthread_mutex_lock(&mutex);
	printf("the resources are %d  %d  %d \n", globalCond[0],globalCond[1],globalCond[2]);
	
	
	rc = pthread_mutex_unlock(&mutex);
	return EXIT_SUCCESS;
}

void* threadCreation(void* args) /*Now Assume that the resource is only one type; TODO: accomodate with multiple resources*/
 {
	 if(!strcasecmp(AVOIDANCE,(char*) args))
	 {
		 printf("start to use banker's algorithm\n");
		 if(Banker(0,1))
		{
			fprintf(stderr, "ERROR: Failure of Banker's Algorithm!!!\n");
			pthread_exit(0); /* exit thread */
		}
	 }else
	 {
		 printf("start to use detection algorithm\n");
	 }
	 pthread_exit(0); /* exit thread */
 }
