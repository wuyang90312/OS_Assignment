#include <stdio.h>
#include <unistd.h>
#define MAX_LINE 80 /* 80 chars per line, per command, should be enough. */

/**
*
* Define a struct type, which can store all of the necessary history information
* 
*****************************************************************************************/
typedef struct{
	int index;
	char *args[MAX_LINE/+1];
	char inputBuffer[MAX_LINE];
}history;
// TODO: Add a read/write protection on struct
// TODO: enable the front space recognition
/*****************************************************************************************
*
*Declare the function
*
**/

void takenRecord(int index, char* args[], char inputBuffer[]);

/*****************************************************************************************
*
*Declare a global variable of history, so all function can access it -- either read or write
*
**/
history record[10];
/*****************************************************************************************/

/**
* setup() reads in the next command line, separating it into distinct tokens
* using whitespace as delimiters. setup() sets the args parameter as a
* null-terminated string.
*/

/* Modification: Added a integer index to indicate the newest command line number*/
void setup(char inputBuffer[], char *args[],int *background, int index)
{
	int length, /* # of characters in the command line */
		i, /* loop index for accessing inputBuffer array */
		start, /* index where beginning of next command parameter is */
		ct, /* index of where to place the next parameter into args[] */
		history; /* indication of whether the command is asking to retrieve history*/

	char headL;

	history = 0;/* when the result of history is (-1||0)/1/2, the corresponding 
	* meaning is that no retrieving/ retrieving the latest one/ retrieving according to the letter*/
	ct = 0;

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

			case '\t' : /* argument separators */
				if(start != -1){
					args[ct] = &inputBuffer[start]; /* set up pointer */
					ct++;
				}
				inputBuffer[i] = '\0'; /* add a null char; make a C string */
				start = -1;
			break;

			/*If there is no other than space in front of 'r', continue */
			case 'r':
				if(history == 0){
					history = 1;
				}

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
				}else{
					/* once the very first letter is not 'r', we know that the 
					 * command is not retrieving; therefore, change the value of history*/
					if (history == 0 || history == 2) history = -1; /*More than 2 letter appearing, invalid case when history = 2 */
					else if ( history == 1){ /* if we have first letter as 'r', check the second letter*/
						history = 2;
						headL = inputBuffer[i];
					}
				}
			}
	}
	args[ct] = NULL; /* just in case the input line was > 80 */

	printf("the headL is %c, the index is %d, the history is %d\n", headL, index, history);
	/*According to history, we decide whether use history record*/
	if( history == 1 ){ /* when history = 1, retrieve the newest record*/
		int ID = (index -1 )% 10; /* once the index is 10, it will take the position of place 0 -- loop */
		
		if(ID < 0){
			printf("ERROR: There is no record existing\n");
		}
		else{
			printf("hello\n");
			/*Copy the most recent data*/
			strcpy(args, record[ID].args);
			strcpy(inputBuffer, record[ID].inputBuffer);
		}
	}else if( history == 2 ){ /* when history = 2, check the first letter of args */
		
	}
}

int main(void)
{
	char inputBuffer[MAX_LINE]; /* buffer to hold the command entered */
	int background,status; /* equals 1 if a command is followed by '&'; status to decide the child status */
	char *args[MAX_LINE/+1]; /* command line (of 80) has max of 40 arguments */
	pid_t fid; /* fid to recognize the parent/child process */
	static int index; /* take record of how many command lines have been passed to the terminal*/

	index = 0;

	while (1){ /* Program terminates normally inside setup */
		background = 0;
		printf(" COMMAND->\n");
		
		setup(inputBuffer, args, &background, index); /* get next command */

		/* Invoke a fork process */
		fid = fork();
		/* If fid is zero, it is child process */
		if(fid == 0){
			/* Test if the statement is valid, otherwise show the error message */
			if(execvp(inputBuffer, args) == -1){
				printf("ERROE: The Command line is invalid\n");
				exit(-1);
			}else{/* After the process finished, kill it*/
				exit(0);
			}
		}
		/* If fid is positive, it is parent process */
		else if(fid > 0){
			/* Here check the background to see if we need to wait the child process to finish */
			if(background == 0){ /* when background = 0, parent process does its own work -- run cocurrently */

			}else{ /* otherwise, parent process waits for child process */
				pid_t tpid ;
				/* keep the while loop running when child signal is not terminated */
				do {
				   tpid = wait(&status);
				   if(tpid != fid) kill(tpid,0);
				 } while(tpid != fid);
			}
		}
		/* otherwise, forking fails */
		else{
			printf("ERROR: forking child process failed\n");
			exit(-1);
		}

		/* the steps are:
		(1) fork a child process using fork()
		(2) the child process will invoke execvp()
		(3) if background == 1, the parent will wait,
		otherwise returns to the setup() function. */
		
		/* Use the status to check if the child process has a valid shell argument */
		if(status > 0 ){ /* only when status > 0, the child run successfully*/
			takenRecord(index, args, inputBuffer);/*After the command line was taken down, put it into histroy*/
			index++; /*everytime a command line is passed, the index is incremented*/
		}
	}
}

void takenRecord(int index, char* args[],char inputBuffer[])
{
	int ID = index % 10; /* once the index is 10, it will take the position of place 0 -- loop */
	/* the record take down all of the values of its variables*/
	record[ID].index = index;
	strcpy(record[ID].args, args);
	strcpy(record[ID].inputBuffer, inputBuffer);
}
