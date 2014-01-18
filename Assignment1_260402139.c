#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#define MAX_LINE 80 /* 80 chars per line, per command, should be enough. */
/**
* setup() reads in the next command line, separating it into distinct tokens
* using whitespace as delimiters. setup() sets the args parameter as a 
* null-terminated string.
*/
static int length;
void setup(char inputBuffer[], char *args[],int *background){
	//int length, /* # of characters in the command line */
		int i, /* loop index for accessing inputBuffer array */
		start, /* index where beginning of next command parameter is */
		ct; /* index of where to place the next parameter into args[] */
	ct = 0;

//Need link list or a huge array.

	// char history[];
	// if(history.length == 0){
	// 	history = inputBuffer;
	// }else{
	// 	int i = history.length;
	// 	for(int k = 0; k < inputBuffer.length ; k++){
	// 		history[i+k] = inputBuffer[k];	// store all the past history of command
	// 	}
	// }
	// printf("History:\n");
	// for(int j = 0; j < history.length ; j++){
	// 	printf("%c\n",history[j] );
	// }


/* read what the user enters on the command line */
	length = read(STDIN_FILENO, inputBuffer, MAX_LINE); 
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
							inputBuffer[i] = '\0'; 
							if(start == i){
								start = -1;
							}
						}
			} 
		} 
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
	//printf("REPEAT\n");
		background = 0;	
		
		printf(" COMMAND->\n");
		setup(inputBuffer,args,&background); /* get next command */
		int s = 0;
		for(s; s < length; s++){
			printf("%s \n",args[s]);
		}
		childPID = fork(); //(1) fork a child process using fork()
		//printf("childPID = %d \n",childPID);
/* the steps are:

(2) the child process will invoke execvp()
(3) if background == 1, the parent will wait, 
otherwise returns to the setup() function. */
		// if fork fail
		if(childPID < 0){
			printf("System Fork Process Fail!!"); // print fail to fork
			exit(1); // exit as fail
		}
		else if(childPID == 0 ){
			//printf("IN Child\n");
			execvp(args[0], args);
			printf("Worng Command\n");
			exit(0);
		}
		else if(childPID > 0){ //Parent Process
			if(background == 0){ // command end without & symbol
				//wait(NULL);

				pid_t currentID = wait(&childStatus);
				while(currentID != childPID){
					currentID = wait(&childStatus);
					
				}

				//return childStatus;

			}
		}
		// else{
		// 	setup(inputBuffer,args,&background);
		// }
		// else{
		// 	printf("IN ELSE\n");
		// 	execvp(args[0], args);
		// 	printf(" COMMAND->\n");
		// 	setup(inputBuffer,args,&background);
		// }
		// else{
		// 	printf("In else sector\n");
		// 	setup(inputBuffer,args,&background); /* get next command */
		// }	

		// else if (childPID ==0){
		// 	if(execvp(args[0], args) == -1){
		// 		printf("Fail to execute command\n");
		// 	}

		// }
		// else{
		// 	if(background == 1){
		// 		wait(NULL);
		// 	}
		// }
		
	}
}