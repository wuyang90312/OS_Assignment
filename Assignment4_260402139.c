/*
Jeffrey Leung
260402139
A4
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#define fail		0
#define done		1
#define DECTECT		2
#define AVOID 		1
#define resouceA 	3
#define resouceB 	2

/*
arguments that specify the deadlock
strategy to use, number of processes, type of resources, range for the number of resources of each type,
amount of time to simulate, etc.


A simulated process (a thread) would come up with a resource request and
submit it to the resource allocator. The resource allocator would grant it if there are resources and there is
no danger of deadlock. The later check is done in the avoidance case.



input:
arg 1: Job name or id
arg 2: strategy used
arg 3: number of process?
arg 4: resource needed / resource id
arg 5: amount of time needed for each process
*/

// pA = true is resouce A is avalible here I initialize for all resources A, B, C
static int num_res, t_num, p_time, p_count=0, r1_aval = 3, r2_aval = 2, x, y, mode, holdFlag = 0;
static int *all_cond;
static int *p_cond;
static pthread_mutex_t mutex_stra = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mutex_det = PTHREAD_MUTEX_INITIALIZER;
static int hold_1[100];
static int hold_2[100];
static int req_A[50];
static int req_B[50];


void* stra_sel(void*);
void sleep_p(int);
int resourceReq(int);
int isSafe(int , int , int , int , int );
int Banker(int , int , int );

int main (int argc, char* argv[]){
	int count_r, i,holdc, attr_int;
	pthread_attr_t attr;
/*
input:
arg 1: Job name or id
arg 2: strategy used
arg 3: number of process?
arg 4: amount of time needed for each process
arg 5+: resource needed / resource id
*/
	for(holdc = 0; holdc < 100; holdc++){
		hold_2[holdc] = 0;
		hold_1[holdc] = 0;
	}
	for(holdc = 0; holdc < 50; holdc++){
		req_A[holdc] = 0;
		req_B[holdc] = 0;
	}

	if(argc < 5){
		fprintf(stderr, "ERROR: Lack of Arguments.\n");
		exit(0);
	}
	num_res = argc-5; // number of different resource requested
	all_cond = malloc(num_res*sizeof(int));
	/*
	Store all the resoureces needed by the process in all_cond
	*/

	int req = 0;
	
	while(req != num_res){
		if(atoi(argv[5+req]) == AVOID){
			req_A[x] = 1;
			x++;
		}else if(atoi(argv[5+req]) == DECTECT){
			req_B[y] = 1;
			y++;
		}
		req++;
		
	}
	mode = atoi(argv[2]);
	for(count_r = 0; count_r < num_res; count_r++){
		all_cond[count_r] = atoi(argv[count_r+3]);
		if(!all_cond[count_r]){
			printf("Wrong arguments. Arguments %d should be a +ve integer", *all_cond+3);
			exit(0);
		}
	}
	/*
	Getting the thread number needed
	*/
	t_num = atoi(argv[3]);
	if(!t_num){
		printf("Error: invalid thread number argument.\n");
		exit(0);
	}
	/*
	Getting the process time of each process
	*/
	p_time = atoi(argv[4]);
	if(!p_time){
		printf("Error: invalid process time argument.\n");
		exit(0);
	}

	p_cond = malloc(t_num*num_res*sizeof(int));
	
	pthread_t thread[t_num];
	

	for(i = 0; i < t_num; i++){

		attr_int = pthread_attr_init(&attr);
		if(attr_int != 0){
			printf("attritbute initialize unsucessful\n");
			exit(0);
		}
		attr_int = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		if(attr_int != 0){
			printf("attritbute initialize unsucessful\n");
			exit(0);
		}
		
		attr_int = pthread_create(&thread[i], &attr, stra_sel, argv[2]);
		if(attr_int != 0){
			printf("Thread %d creation unsucessful\n",i);
			exit(0);
		}
		// sleep for appropiate time after each process got their resource allocated
		sleep(p_time);
	}
	
	
	printf("succuess\n");
	exit(done);
}

/*
This method detect if input for mode is input correctly or not and keep count of which process is the thread at.
*/
void* stra_sel(void* args){
	int lock, granted;
	
	if(mode!= AVOID && mode!= DECTECT){
		printf("Invalid mode argument input\n");
	}
	lock = pthread_mutex_lock(&mutex_stra);
	p_count++;
	lock = pthread_mutex_unlock(&mutex_stra);
	granted = resourceReq(mode);

}

/*
Sleep for input amount of time
*/
void sleep_p(int process_sleep){
	sleep(process_sleep);
}

int isSafe(int resource_1_aval, int resource_2_aval, int c_process, int needed_1, int needed_2){
	int finish[t_num];
	int k;
	for(k = 0; k< t_num;k++){
		finish[k] = 0;
	}
	if(t_num == -1){
		printf("No more process need to work on\n");
		return done;
	}else if(t_num < -1){
		printf("Error on the number of process Please check the internal methods\n");
		exit(fail);
	}
	if(needed_1 <= resource_1_aval && needed_2 <= resource_2_aval){

	}
}

/*
This is the banker algorithm. 
If there is avalible resources, this method will grant the use of it. 
Otherwise, hold_1 and hold_2 array store process ID and needed resources for type A and B respectively. This is done by stored needed resouces for type A at hold_1 (B for hold_2)
at index of the current process ID.
*/
int Banker(int req_1, int req_2, int c_proID){
	
	if(hold_1[c_proID] == 0 && hold_2[c_proID] == 0){
		// only print onces for earch proces ID
		printf("Process %d request number of \nA: %d, B: %d.\nAvalible A: %d B: %d.\n",c_proID,req_1,req_2,r1_aval, r2_aval );
	}
	if(req_1 > resouceA || req_2 > resouceB ){
		printf("Process %d request more resources than maximum resource\n", c_proID);
		exit(fail);
	}else if(hold_1[c_proID] != 0 || hold_2[c_proID] != 0){
		if(req_1 > r1_aval || req_2 > r2_aval){
			// when needed resources is within hold arrays but resources still not avalible.
			Banker(0,0, c_proID);
		}else{
			// when resources avalible for holded process
		req_1 = hold_1[c_proID];
		req_2 = hold_2[c_proID];

		// add back the req value from previous process after its process time (sleep time after thread create in main)
		r1_aval += req_1;
		r2_aval += req_2;

		printf("Process %d on hold.\nProcess %d request number of \nA: %d, B: %d.\nAvalible A: %d B: %d.\n", c_proID,c_proID,hold_1[c_proID],hold_2[c_proID],r1_aval, r2_aval );


		r1_aval -= req_1;
		r2_aval -= req_2;

		hold_1[c_proID] = 0;
		hold_2[c_proID] = 0;
		printf("Process %d have been granted resources A: %d, B: %d\n", c_proID, req_1, req_2);
		return 1;
		}
	}else if(req_1 > r1_aval || req_2 > r2_aval){
		// not enough resources and store in hold array, run banker again until resource avalible
		hold_1[c_proID] = req_1;
		hold_2[c_proID] = req_2;
		Banker(0,0, c_proID);

	}else{
		// when resource is avalible for the first time
		r1_aval -= req_1;
		r2_aval -= req_2;
		hold_1[c_proID] = 0;
		hold_2[c_proID] = 0;
		printf("Process %d have been granted resources A: %d, B: %d\n", c_proID, req_1, req_2);

		if(r1_aval < req_1 || r2_aval < req_2){
			holdFlag = 1;
			}
		return 1;
	}
}


/*
The detection will keep on running within the theread and the the freequency of how often it check deadlock will base on each process time.
Here if there is deadlock, it will do a recursion of itself until resources is avalible.
*/
int dectection(int reqA, int reqB){
	int sign;

	printf("Process %d request number of \nA: %d, B: %d.\nAvalible A: %d B: %d.\n",p_count,x,y,r1_aval, r2_aval );
	if(reqA > resouceA || reqB > resouceB ){
		printf("Process %d request more resources than maximum resource\n", c_proID);
		exit(fail);
	}
	if(reqA > r1_aval || reqB > r2_aval){
			sign = pthread_mutex_lock(&mutex_det);
			holdFlag = 1;
			printf("Detect not enough resources for process %d\n", p_count);
			// since at this point resources is unavlible but after other previous process finish, (same runtime) it resource will be back (after the sleep_p at main after ptherad create)
			// so add back resource here
			r1_aval += reqA;
			r2_aval += reqB;
			sign = pthread_mutex_unlock(&mutex_det);
			dectection(reqA, reqB);
	}else{
		// when avalible
		r1_aval -= reqA;
		r2_aval -= reqB;
		printf("Process %d have been granted resources A: %d, B: %d\n", p_count, reqA, reqB);
		return 1;
	}
}
//Sort into different mode
int resourceReq(int mode){
	if(mode == 1){
		int Bank_sucess = 0;
		Bank_sucess = Banker(x, y, p_count);
		if(Bank_sucess == 0){
			pthread_exit(NULL);
			return 0;
		}
		return 1;
	}else if(mode == 2){
		int det = dectection(x,y);
	}else{
		printf("Unknown mode!!!!\n");
	}
}






