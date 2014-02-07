#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <regex.h>
#define MAX_LINE 80 /* 80 chars per line, per command, should be enough. */
#define BUFFER_SIZE 50

static pid_t childPID;
static char buffer[BUFFER_SIZE];
static int and = 0;
/**
* setup() reads in the next command line, separating it into distinct tokens
* using whitespace as delimiters. setup() sets the args parameter as a 
* null-terminated string.
*/
int length;
int error = 0, his = 0;
int s = 0, q = 0, j = 0; // index for history array
int RFlag = 0, fz =0;
char history[100][80]; // 2D array offer



void setup(char inputBuffer[], char *args[],int *background){
	//int length, /* # of characters in the command line */
		int i, /* loop index for accessing inputBuffer array */
		start, /* index where beginning of next command parameter is */
		ct; /* index of where to place the next parameter into args[] */
		ct = 0;
		RFlag = 0;

	
/* read what the user enters on the command line */
	if(his == 0){
		length = read(STDIN_FILENO, inputBuffer, MAX_LINE); 
	}
	his = 0;
	if(inputBuffer[i] != 'r'){
		for (q = 99;q>0; q--){
  			strcpy(history[q], history[q-1]);
 			strcpy(history[0],inputBuffer);
 		}
 	}
	start = -1;
	if (length == 0)
		exit(0); /* ^d was entered, end of user command stream */
	if (length < 0){
		perror("error reading the command");
		exit(-1); /* terminate with error code of -1 */
	}
/* examine every character in the inputBuffer */
		for (i=0;i<length;i++) { 
			switch (inputBuffer[i]){
				case ' ':
				case '\t' :    /* argument separators */
					if(start != -1){
						args[ct] = &inputBuffer[start]; /* set up pointer */
						ct++;
					}
					inputBuffer[i] = '\0'; /* add a null char; make a C string */
					start = -1;
					break;
				case '\n': /* should be the final char examined */
					if (start != -1){
						args[ct] = &inputBuffer[start]; 
						ct++;
					}
					inputBuffer[i] = '\0';
					args[ct] = NULL; /* no more arguments to this command */
					break;
				default : /* some other character */
					if (start == -1)
						start = i;
						if (inputBuffer[i] == '&'){
							*background = 1;
							and++;
							inputBuffer[i] = '\0'; 
							if(start == i){
								start = -1;
							}
						}
					if(inputBuffer[0] == 'r'){
						if(inputBuffer[i+2] == 0){
							RFlag = 1;
							//printf("RFlag: %d\n", RFlag);
						}else if(strlen(inputBuffer) <=4){
							RFlag = 2;
							//printf("RFlag: %d\n",RFlag );
						}
						start = i;
					}
			} 
		} 
		q++;
	args[ct] = NULL; /* just in case the input line was > 80 */
} 


void handle_SIGINT(){  // control C
	fprintf(stderr, "\n" );
	//printf("In ctrl c\n");
    //exit(0);
}

void handle_SIGTSTP(){  // control Z
	fprintf(stderr, "\n" );
	//printf("in ctrl z\n");
	fz = 1;
    kill(childPID, SIGSTOP);
}

int main(void){



	char inputBuffer[MAX_LINE]; /* buffer to hold the command entered */
	int background; /* equals 1 if a command is followed by '&' */
	char *args[MAX_LINE/+1];  /* command line (of 80) has max of 40 arguments */
	int i = 0;
	int childStatus;

	
	fz = 0;
    struct sigaction handler;
   	handler.sa_handler = handle_SIGINT;
   	sigaction(SIGINT, &handler, NULL);
    handler.sa_handler = handle_SIGTSTP;
    sigaction(SIGTSTP, &handler,NULL);
 	int RDFLAG = 0;

	while (1){    /* Program terminates normally inside setup */
		int buffCount = 0;
		background = 0;	
		if(RFlag != 1 || RFlag != 2){
			childPID = fork(); //(1) fork a child process using fork()
		}
		//printf("inputBuffer:%s\n",&inputBuffer[0]);
		//printf("args :%s\n", args[1]);
		if(!strcmp(inputBuffer,"exit") && args[1] == NULL){	// just r
			exit(0);
		}
		else if(RFlag == 1){
			RFlag = 0;
				strcpy(inputBuffer, history[0]);
				RDFLAG = 0;
				//his = 1;
 				system(&inputBuffer[0]);		
			  	
		}else if (RFlag == 2){ // when it is not just r

			RFlag = 0;
			int a=0;
			for(a = 0;a < 99; a++){
				char st =history[a][0];
				if(st == inputBuffer[2]){
					break;
				}
			}
			//his = 1;
			RDFLAG = 0;
 			system(history[a]);
		}
		/***************************************/
		// cd Command
		else if(!strcmp(inputBuffer,"cd") && args[2] == NULL){ // CD to the directory
					chdir(args[1]);
				}
		/**************************************/
		else if(!strcmp(inputBuffer,"history") && args[1] == NULL){	//print out all the history
			int count = 99-q;
			int zcount = 0;
			printf("History lenght %lu\n", sizeof(&history));
			printf("%d\n",q );
			while(zcount !=  10){
				printf("[%d]: %s\n", zcount+1,history[zcount+2]);
				zcount++;
			}
		}
		else if(!strcmp(inputBuffer,"jobs") && args[1] == NULL){ // identify jobs typed into the command
			int h = 1;
			int ID = 0;
			printf("In Jobs\n");
			printf("PID %d\n",getpid());
			printf("ID %d\n",ID );
			printf("childPID %d\n",childPID );
				if(childPID == 0){
					while(ID != getpid()){
						ID = getpid();
						printf("[%d] ID: %d\n",h,ID );	// print out the PID
						h++;
					}
				}
		}
		else if(childPID < 0){
			printf("System Fork Process Fail!!"); // print fail to fork
			exit(1); // exit as fail
		}
		else if(childPID == 0 ){	// child process
			if(execvp(args[0], args) == -1){
				error = -1;
			}
			execvp(args[0], args);
			printf("Worng Command\n");
			exit(0);
		}
		else if(childPID > 0){ //Parent Process
			if(background == 0){ // command end without & symbol
				if(!strcmp(inputBuffer,"fg")){

				}
				pid_t currentID = wait(&childStatus);
				while(currentID != childPID){
					currentID = wait(&childStatus);
				}
			}else{
				if(strcmp(inputBuffer,"fg")){

				}
			}
		}
		
			printf(" COMMAND->\n");
		
		his = 0;
		setup(inputBuffer,args,&background); /* get next command */
	}
}