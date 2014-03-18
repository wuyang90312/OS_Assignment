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
#define SUCCESS		1
#define FAILURE		0/**
* Purpose: Solve assignment 4 -- Deadlock Avoidance/Detection
* Author: Yang Wu -- yang.wu2@mail.mcgill.ca
* Date: March 17th 2014*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define AVOIDANCE	"avoidance"
#define DETECTION	"detection"
#define SUCCESS		1
#define FAILURE		0
#define SEED		3
#define MINUS		0
#define PLUS		1
#define TRUE		1

static int threadNum, resourceType, process, processTime;
static int *globalCond;
static int *processCond;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mutexNUM = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mutexCOND = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mutexADD = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;	


/* For testing purpose */
int processlimit[4][3] ={{4,4,4},{4,4,4},{4,4,4},{4,4,4}}; 
 
/*
 *********************Declaration of all the classes********************/
void* threadCreation(void*);
void sleepProcess();
 void globalCondModify(int, int, int);
int checkSafty();
int Banker(int, int,int);
int resourceRequest(int, int, int, int);
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
	
	processTime = atoi(argv[arg-1]);
	if(!processTime)
	{
		fprintf(stderr, "ERROR: Invalid of Argument: \nArgument %d is supposed to be a positive integer\n", (arg-1));
		return EXIT_FAILURE;
	}
	printf("the time is %d\n", processTime);
	processCond = malloc(resourceType*threadNum*sizeof(int)); /* create a */
	/*Create a pthread according to the set number*/
	pthread_t thread[threadNum];
	for( loop =0; loop <resourceType*threadNum; loop++) //TODO: clean up the for loop
	{
		processCond[loop] = processlimit[loop/3][loop%3];
	}

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
	sleep(processTime);
	return EXIT_SUCCESS;
}

int Banker(int currentprocess, int type, int resource) /*The whole section is set as a critical session so that only one process can access the Banker at the same time*/
{
	int rc, safty;
	rc = pthread_mutex_lock(&mutex);
	printf("The %d-%d\t", currentprocess, type);
	printf("1. the resources are %d  %d  %d \t", globalCond[0],globalCond[1],globalCond[2]);
	
	//globalCond[type] -= resource;
	globalCondModify(MINUS,resource,type);
	processCond[currentprocess*resourceType+type] -=resource;
	
	safty = checkSafty(); /* check the safty of the resources*/
	if(!safty) /* Once the whole system is not safe after assignment, reject the action, back up the old data */
	{
		//globalCond[type] += resource;
		globalCondModify(PLUS,resource,type);
		processCond[currentprocess*resourceType+type] +=resource;
	}
	printf("2. the resources are %d  %d  %d --> %d\n", globalCond[0],globalCond[1],globalCond[2],safty);
	rc = pthread_mutex_unlock(&mutex);
	return safty;
}

int checkSafty()
{	
	int loop, succession, quotient; 
	loop = 0;
	succession = 0;

	while(loop < resourceType*threadNum)
	{
		if(globalCond[loop%resourceType] >= processCond[loop])
		{
			//printf("%d >= %d\t", globalCond[loop%resourceType], processCond[loop]);
			succession ++;
			if(succession == 3)
			{
				return SUCCESS;
			}
			loop ++;
		}else
		{
			succession = 0;
			quotient = (int) (loop / resourceType);
			loop = (quotient+1)*resourceType;
		}
			
	}
	
	return FAILURE;
}

void* threadCreation(void* args) /*Now Assume that the resource is only one type; TODO: accomodate with multiple resources*/
 {
	 int algorithm, currentprocess,rc, resourceT, content, type[resourceType], resource[resourceType];

	 
	 rc = pthread_mutex_lock(&mutexNUM); /* Index each of the processes*/
	 currentprocess = process;
	 process ++;
	 rc = pthread_mutex_unlock(&mutexNUM);

	 if(!strcasecmp(AVOIDANCE,(char*) args))
	 {
		 algorithm = 0;
	 }else
	 {
		 algorithm = 1;
	 }
	/* sleepProcess();
	 type[0] = 0;
	 resource[0] = processlimit[currentprocess][0];
	 if(!resourceRequest(currentprocess, algorithm, type[0], resource[0]))
	 {
		fprintf(stderr, "ERROR: Failure of Resource Allocation!!!\n");
		pthread_exit(0);
	 }
	sleepProcess();
	 type[1] = 1;
	 resource[1] = processlimit[currentprocess][1];
	 if(!resourceRequest(currentprocess, algorithm, type[1], resource[1]))
	 {
		fprintf(stderr, "ERROR: Failure of Resource Allocation!!!\n");
		pthread_exit(0);
	 }
	sleepProcess();
	 type[2] = 2;
	 resource[2] = processlimit[currentprocess][2];
	 if(!resourceRequest(currentprocess, algorithm, type[2], resource[2]))
	 {
		fprintf(stderr, "ERROR: Failure of Resource Allocation!!!\n");
		pthread_exit(0);
	 }*/

	/* keeping the thread running until program terminate itself */
	while(TRUE)
	{
		int seed = time(NULL);
		srand(seed);
		resourceT = rand()%resourceType;
		content = 0;
		while(!content)
		{
			content = rand()%processCond[resourceT]+1;
		}
		if(!resourceRequest(currentprocess, algorithm, resourceT, resource[2]))
		{
			fprintf(stderr, "ERROR: Failure of Resource Allocation!!!\n");
			pthread_exit(0);
		}
		sleepProcess();
		
		globalCondModify(PLUS, content, resourceT);
		pthread_cond_broadcast(&cond); /* once resource are released, wake up all the sleeping threads*/
		printf("%d sucessfully start to signal, resource: %d %d %d \n", currentprocess,globalCond[0],globalCond[1],globalCond[2]);
	}

	/* globalCond[0] += resource[0];
	 globalCond[1] += resource[1];
	 globalCond[2] += resource[2];*/
	// pthread_cond_broadcast(&cond); /* once resource are released, wake up all the sleeping threads*/
	 //printf("%d sucessfully start to signal, resource: %d %d %d \n", currentprocess,globalCond[0],globalCond[1],globalCond[2]);
	 pthread_exit(0); /* exit thread */
 }
 
 int resourceRequest(int currentprocess, int algorithm, int type, int resource)
 {
	 if(algorithm){
		
	 }else
	 {
		 while(!Banker(currentprocess, type, resource))
		{
			//printf(" The %d-%d is stuck for signal \n", currentprocess, type);
			pthread_cond_wait(&cond, &mutexCOND);
			pthread_mutex_unlock(&mutexCOND);
			
		}
	 }

	 return SUCCESS;
 }
 
 void globalCondModify(int type, int content, int index) /* Make sure that only one process modify the global variable each time*/
 {
	 pthread_mutex_lock(&mutexADD);
	 if(type)
		globalCond[index] += content;
	 else
		globalCond[index] -= content;
	 pthread_mutex_unlock(&mutexADD);
 }
 
 void sleepProcess()
 {
	 srand(SEED);
	 int sleeptime = rand()%5+1;/* Assume the maximal sleeping time is 4 seconds*/
	 //printf("The sleeping time is %d\n", sleeptime);
	 sleep(sleeptime);
 }

#define SEED		3
#define MINUS		0
#define PLUS		1

static int threadNum, resourceType, process, processTime;
static int *globalCond;
static int *processCond;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mutexNUM = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mutexCOND = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mutexADD = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;	


/* For testing purpose */
int processlimit[4][3] ={{4,4,4},{4,4,4},{4,4,4},{4,4,4}}; 
 
/*
 *********************Declaration of all the classes********************/
void* threadCreation(void*);
void sleepProcess();
 void globalCondModify(int, int, int);
int checkSafty();
int Banker(int, int,int);
int resourceRequest(int, int, int, int);
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
	
	processTime = atoi(argv[arg-1]);
	if(!processTime)
	{
		fprintf(stderr, "ERROR: Invalid of Argument: \nArgument %d is supposed to be a positive integer\n", (arg-1));
		return EXIT_FAILURE;
	}
	printf("the time is %d\n", processTime);
	processCond = malloc(resourceType*threadNum*sizeof(int)); /* create a */
	/*Create a pthread according to the set number*/
	pthread_t thread[threadNum];
	for( loop =0; loop <resourceType*threadNum; loop++) //TODO: clean up the for loop
	{
		processCond[loop] = processlimit[loop/3][loop%3];
	}

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
	sleep(processTime);
	return EXIT_SUCCESS;
}

int Banker(int currentprocess, int type, int resource) /*The whole section is set as a critical session so that only one process can access the Banker at the same time*/
{
	int rc, safty;
	rc = pthread_mutex_lock(&mutex);
	//printf("The %d-%d\t", currentprocess, type);
	//printf("1. the resources are %d  %d  %d \t", globalCond[0],globalCond[1],globalCond[2]);
	
	//globalCond[type] -= resource;
	globalCondModify(MINUS,resource,type);
	processCond[currentprocess*resourceType+type] -=resource;
	
	safty = checkSafty(); /* check the safty of the resources*/
	if(!safty) /* Once the whole system is not safe after assignment, reject the action, back up the old data */
	{
		//globalCond[type] += resource;
		globalCondModify(PLUS,resource,type);
		processCond[currentprocess*resourceType+type] +=resource;
	}
	//printf("2. the resources are %d  %d  %d --> %d\n", globalCond[0],globalCond[1],globalCond[2],safty);
	rc = pthread_mutex_unlock(&mutex);
	return safty;
}

int checkSafty()
{	
	int loop, succession, quotient; 
	loop = 0;
	succession = 0;

	while(loop < resourceType*threadNum)
	{
		if(globalCond[loop%resourceType] >= processCond[loop])
		{
			//printf("%d >= %d\t", globalCond[loop%resourceType], processCond[loop]);
			succession ++;
			if(succession == 3)
			{
				return SUCCESS;
			}
			loop ++;
		}else
		{
			succession = 0;
			quotient = (int) (loop / resourceType);
			loop = (quotient+1)*resourceType;
		}
			
	}
	
	return FAILURE;
}

void* threadCreation(void* args) /*Now Assume that the resource is only one type; TODO: accomodate with multiple resources*/
 {
	 int algorithm, currentprocess,rc, type[resourceType], resource[resourceType];

	 
	 rc = pthread_mutex_lock(&mutexNUM); /* Index each of the processes*/
	 currentprocess = process;
	 process ++;
	 rc = pthread_mutex_unlock(&mutexNUM);

	 if(!strcasecmp(AVOIDANCE,(char*) args))
	 {
		 algorithm = 0;
	 }else
	 {
		 algorithm = 1;
	 }
	 sleepProcess();
	 type[0] = 0;
	 resource[0] = processlimit[currentprocess][0];
	 if(!resourceRequest(currentprocess, algorithm, type[0], resource[0]))
	 {
		fprintf(stderr, "ERROR: Failure of Resource Allocation!!!\n");
		pthread_exit(0);
	 }
	sleepProcess();
	 type[1] = 1;
	 resource[1] = processlimit[currentprocess][1];
	 if(!resourceRequest(currentprocess, algorithm, type[1], resource[1]))
	 {
		fprintf(stderr, "ERROR: Failure of Resource Allocation!!!\n");
		pthread_exit(0);
	 }
	sleepProcess();
	 type[2] = 2;
	 resource[2] = processlimit[currentprocess][2];
	 if(!resourceRequest(currentprocess, algorithm, type[2], resource[2]))
	 {
		fprintf(stderr, "ERROR: Failure of Resource Allocation!!!\n");
		pthread_exit(0);
	 }

	/* keeping the thread running until program terminate itself */
	while(TRUE)
	{
		int seed = time(NULL);
		srand(seed);
		int resourceT = rand()%resourceType;
		if(!resourceRequest(currentprocess, algorithm, resourceT, resource[2]))
		{
			
		}
		sleepProcess();
	}

	 globalCond[0] += resource[0];
	 globalCond[1] += resource[1];
	 globalCond[2] += resource[2];
	 pthread_cond_broadcast(&cond); /* once resource are released, wake up all the sleeping threads*/
	 printf("%d sucessfully start to signal, resource: %d %d %d \n", currentprocess,globalCond[0],globalCond[1],globalCond[2]);
	 pthread_exit(0); /* exit thread */
 }
 
 int resourceRequest(int currentprocess, int algorithm, int type, int resource)
 {
	 if(algorithm){
		
	 }else
	 {
		 while(!Banker(currentprocess, type, resource))
		{
			//printf(" The %d-%d is stuck for signal \n", currentprocess, type);
			pthread_cond_wait(&cond, &mutexCOND);
			pthread_mutex_unlock(&mutexCOND);
			
		}
	 }

	 return SUCCESS;
 }
 
 void globalCondModify(int type, int content, int index) /* Make sure that only one process modify the global variable each time*/
 {
	 pthread_mutex_lock(&mutexADD);
	 if(type)
		globalCond[index] += content;
	 else
		globalCond[index] -= content;
	 pthread_mutex_unlock(&mutexADD);
 }
 
 void sleepProcess()
 {
	 srand(SEED);
	 int sleeptime = rand()%5+1;/* Assume the maximal sleeping time is 4 seconds*/
	 printf("The sleeping time is %d\n", sleeptime);
	 sleep(sleeptime);
 }
