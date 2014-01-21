#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#define MAX_LINE 80 /* 80 chars per line, per command, should be enough. */
/**
* setup() reads in the next command line, separating it into distinct tokens
* using whitespace as delimiters. setup() sets the args parameter as a 
* null-terminated string.
*/
int length;
int error = 0, his = 0;
int s = 0, q = 0, j = 0; // index for history array
int RFlag = 0;
char history[100][80];

void setup(char inputBuffer[], char *args[],int *background){
	//int length, /* # of characters in the command line */
		int i, /* loop index for accessing inputBuffer array */
		start, /* index where beginning of next command parameter is */
		ct; /* index of where to place the next parameter into args[] */
		ct = 0;
	
/* read what the user enters on the command line */
	if(his == 0){
		length = read(STDIN_FILENO, inputBuffer, MAX_LINE); 
	}
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
				case 'r':
					if(inputBuffer[i+2] == 0){
						RFlag = 1;
						printf("RFlag: %d\n", RFlag);
					}else{
						RFlag = 2;
						printf("RFlag: %d\n",RFlag );
					}
					break;
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
							inputBuffer[i] = '\0'; 
							if(start == i){
								start = -1;
							}
						}
			} 
		} 
		i = his;
	args[ct] = NULL; /* just in case the input line was > 80 */
} 

int main(void){
	char inputBuffer[MAX_LINE]; /* buffer to hold the command entered */
	int background; /* equals 1 if a command is followed by '&' */
	char *args[MAX_LINE/+1];  /* command line (of 80) has max of 40 arguments */
	pid_t childPID;
	int i = 0;
	int childStatus;
	while (1){    /* Program terminates normally inside setup */
		int buffCount = 0;
		background = 0;	
		printf(" COMMAND->\n");
		setup(inputBuffer,args,&background); /* get next command */
		if(RFlag != 1 || RFlag != 2){
			childPID = fork(); //(1) fork a child process using fork()
		}
		if(RFlag == 1){
			RFlag = 0;
			strcpy(inputBuffer, history[i]);
 			system(inputBuffer);  			
		}else if (RFlag == 2){
			RFlag = 0;
			int a=0;
			printf("BUF %s\n",&inputBuffer[2]);
			printf("history 11 %s\n",&history[a][0]  );
			// while(&history[a][0] != &inputBuffer[2]){
			// 	a++;
			// }
			strcpy(inputBuffer, history[a]);
 			printf("buffer %s\n",inputBuffer);
 			printf("history %s\n",&history[a][0] );
 			system(history[a]);
		}
		else if(childPID < 0){
			printf("System Fork Process Fail!!"); // print fail to fork
			exit(1); // exit as fail
		}
		else if(childPID == 0 ){
			//printf("IN Child\n");
			if(execvp(args[0], args) == -1){
				error = -1;
			}
			execvp(args[0], args);
			printf("Worng Command\n");
			exit(0);
		}
		else if(childPID > 0){ //Parent Process
			if(background == 0){ // command end without & symbol
				pid_t currentID = wait(&childStatus);
				while(currentID != childPID){
					currentID = wait(&childStatus);
					
				}

			}
		}
		
	}
}