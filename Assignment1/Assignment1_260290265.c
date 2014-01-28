#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_LINE 80 /* 80 chars per line, per command, should be enough. */
#define HISTORY_LIMIT 20
#define RECORD_LIMIT 10
#define BG_LIMIT 5

/**
*
* Define a struct type, which can store all of the necessary history information
* 
*****************************************************************************************/
typedef struct history{
	int index;
	char *args[MAX_LINE/+1];
	char inputBuffer[MAX_LINE];
}history;

/* Define another struct to take record of background process */
typedef struct jobs{
	int    index;
	pid_t  pid;
	char *args[MAX_LINE/+1];
	char inputBuffer[MAX_LINE];
	struct job *last;
	struct job *next;
}jobs;
// TODO: reset the history
/*****************************************************************************************
*
*Declare the function
*
**/

void takenRecord(int index, char* args[], char inputBuffer[], history record[], int limit);
int recordWLetter(int index, int *position, char letter);
void positionCalculation(char* args[], char* argss[], char* input);

/*****************************************************************************************
*
*Declare a global variable of history, so all function can access it -- either read or write
*
**/
static history record[RECORD_LIMIT];
static history History[HISTORY_LIMIT]; /* take down the most recent 30 commands*/
static jobs job[BG_LIMIT]; /* Maximum take 5 records of background running programs*/
static int childID, flagZ;
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
	int ID = (index -1 )% RECORD_LIMIT; /* once the index is 10, it will take the position of place 0 -- loop */
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
					if(inputBuffer[i] == 'r' && history == 0 ){ /*If there is no other than space in front of 'r', continue */
						history = 1;
					}
					else if (history == 0 || history == 2){ /* More than 2 letter appearing, invalid case when history = 2 */
						 history = -1;
					 }
					else if ( history == 1){ /* if we have first letter as 'r', check the second letter*/
						history = 2;
						headL = inputBuffer[i];
					}
				}
			}
	}
	args[ct] = NULL; /* just in case the input line was > 80 */


	/*According to history, we decide whether use history record*/
	if( history == 1 ){ /* when history = 1, retrieve the newest record*/
		
		if(ID < 0){
			printf("ERROR: There is no record existing\n");
		}
		else{
			/*Copy the most recent data*/
			memcpy(inputBuffer, record[ID].inputBuffer, MAX_LINE);
			positionCalculation( record[ID].args, args, &inputBuffer[0]); /* calculate new memory location*/
		}
		}else if( history == 2 ){ /* when history = 2, check the first letter of args */
			/* if the function returns zero, error occurs or no fitting command existing*/
			if(recordWLetter(index, &ID, headL)){
				/*Copy the most recent data*/
				memcpy(inputBuffer, record[ID].inputBuffer, MAX_LINE);
				positionCalculation( record[ID].args, args, &inputBuffer[0]); /* calculate new memory location*/
		}
	}
}

int recordWLetter(int index, int *position, char letter)
{
	int counter; /* allocate a counter to count up to 10 */
		counter = 0;

		while(index >= 0){ /* if the index is running out of limit, stop while loop and return fault */
				*position = (index - counter-1) % RECORD_LIMIT;
				if(counter >9){ /* if all of the 10 array indices have been run, return fault*/
						return 0;
				}else if( record[*position].inputBuffer[0] == letter){
						return 1;
				}
				counter ++;
		}

		return 0;
}

/* handle the ctrl-C */
void handle_SIGINT() {
	/* Do nothing when ctrl-c is signalled */
	fprintf( stderr, "\n");
	exit(0);
}

/* handle the ctrl-Z */
void handle_SIGTSTP() {
	fprintf( stderr, "\n");
	flagZ = 1;
	kill(childID, SIGSTOP);
	//kill(childID, SIGCONT);
}

int main(void)
{
/********************************************************************************************************
** set up the signal handler */
	if (signal(SIGINT, SIG_IGN) != SIG_IGN) /*Disable the Ctrl-C*/
		signal(SIGINT, handle_SIGINT);
	if (signal(SIGTSTP, SIG_IGN) != SIG_IGN) /*Disable the Ctrl-Z*/
		signal(SIGTSTP, handle_SIGTSTP);
	/* initialize the ctrl-Z flag*/
	flagZ = 0;
/*********************************************************************************************************/

	char inputBuffer[MAX_LINE]; /* buffer to hold the command entered */
	int background,status, flag; /* equals 1 if a command is followed by '&'; status to decide the child status */
	char *args[MAX_LINE/+1]; /* command line (of 80) has max of 40 arguments */
	pid_t fid; /* fid to recognize the parent/child process */
	static int index; /* take record of how many command lines have been passed to the terminal*/
	int Hindex = 0; /* History index */
	jobs* MostRecent;

	flag = 0;
/**********************************************************************************************************
 * Set up a loop link-list */
	int a;
	job[0].last = &job[4];
	job[0].next = &job[1];
	for(a = 1; a < (BG_LIMIT-1); a++){
		job[a].last = &job[a-1];
		job[a].next = &job[a+1];
	}
	job[4].last = &job[3];
	job[4].next = &job[0];
	MostRecent = &job[0];

/*********************************************************************************************************/

	index = 0;
	while (1){ /* Program terminates normally inside setup */
		background = 0;
		printf(" COMMAND->\n");

		setup(inputBuffer, args, &background, index); /* get next command */

		/* Take down all of the history record */
		takenRecord(Hindex, args, inputBuffer, History, HISTORY_LIMIT);
		Hindex++; /*everytime a command line is passed, the index is incremented*/
		/* Invoke a fork process */
		fid = fork();
		/* If fid is zero, it is child process */
		if(fid == 0){
			if(!strcmp(inputBuffer,"exit") && args[1] == NULL){ /* If the input has only exit, end child process */
				exit(0);
			}
			else if(!strcmp(inputBuffer,"cd") && args[2] == NULL){ /* If the input has cd and the directory, go to the directory
			and then end the process */
				int ret = chdir(args[1]); /* when the directory is wrong, ret = -1 */
				exit(ret);
			}
			else if(!strcmp(inputBuffer,"jobs") && args[2] == NULL){
				jobs* position = MostRecent->next;
				int a;
				/*check if the background process still exit*/
				for(a =0; a<BG_LIMIT; a++){
					if(position->pid){
						printf("%d.  ", position->index);
						int i =0;
						while(position->args[i]!= NULL){
							printf("%s ",position->args[i]);
							i++;
						}
						printf("    running\n");
					}
					position = position->next;
				}
			}
			else if(!strcmp(inputBuffer,"fg")&& args[2] == NULL){ /* Run the most recent bg*/
				jobs* pt;
				pt = MostRecent;
				//printf("the %d   %s", pt->pid, pt->args[0]);
				while(!pt->pid){
					pt = pt->last;
				}
				printf("%d.  ", pt->index);
				int i =0;
				while(MostRecent->args[i]!= NULL){
					printf("%s ",pt->args[i]);
					i++;
				}
				printf("    running\n");
			}
			else if(!strcmp(inputBuffer,"history") && args[1] == NULL){
				int i;
				for(i=0; i < HISTORY_LIMIT; i++)
				{
					int Indx = 0;
					if(History[i].args[Indx] == NULL)  break;
					printf("%d    ", History[i].index );
					while(History[i].args[Indx] != NULL){
						printf("%s ", History[i].args[Indx]);
						Indx++;
					}
					printf("\n");
				};
			}
			/* Test if the statement is valid, otherwise show the error message */
			else if(execvp(inputBuffer, args) == -1){
				printf("ERROR: The Command line is invalid\n");
				exit(-1);
			}
			/* After the process finished, kill it*/
			exit(0);
		}
		/* If fid is positive, it is parent process */
		else if(fid > 0){

			childID = fid;
			pid_t	tpid;
			
			/* Here check the background to see if we need to wait the child process to finish */
			if(background){ /* when background = 1, parent process does its own work -- run cocurrently */
				if(strcmp(inputBuffer,"fg")){
					MostRecent = MostRecent->next; /* set the MostRecent to next position*/
					MostRecent->pid = fid; /* STORE the command into job */
					memcpy(MostRecent->inputBuffer, inputBuffer, MAX_LINE); /* duplicate inputBUffer*/
					positionCalculation( args, MostRecent->args, &MostRecent->inputBuffer[0]); /* calculate new memory location*/
					if(flag<5)flag++;
				}
			}else{ /* otherwise, parent process waits for child process */
				if(!strcmp(inputBuffer,"fg")){/* once command is "fg", change the pid to the stored pid in struct */
					jobs* pt;
					pt = MostRecent;
					while(!pt->pid){
						pt = pt->next;
					}
					fid = pt->pid;
				}
				/* wait untill when child signal is not terminated */
				do {
					tpid = waitpid(fid, &status, WSTOPPED);
				} while(tpid != fid);

				if(flagZ) {
					kill(fid, SIGCONT); /* Resume the child process at background*/
					flagZ = 0;
					if(strcmp(inputBuffer,"fg"))
					{
						MostRecent = MostRecent->next; /* set the MostRecent to next position*/
						MostRecent->pid = fid; /* STORE the command into job */
						memcpy(MostRecent->inputBuffer, inputBuffer, MAX_LINE); /* duplicate inputBUffer*/
						positionCalculation( args, MostRecent->args, &MostRecent->inputBuffer[0]); /* calculate new memory location*/
						if(flag<5)flag++;
					}
				}
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

		int   num = 1;
		jobs* position = MostRecent;
		/*check if the background process still exit*/
		for(a =0; a<BG_LIMIT; a++){
			position = position->next;
			//printf("The memory addres is %x, flag is %d, %s the status is %d \n", position,flag, position->args[1],waitpid(position->pid, &status, WNOHANG));
			if(!waitpid(position->pid, &status, WNOHANG) && position->pid != 0)
			{
				position->index = num;
				//printf("The index is %d, The process %d, input is %s  %s\n",position->index, position->pid,position->args[0], position->args[1]);
				num++;
			}
			else{
				if(position->pid != 0)flag--;
				//printf("The process %d has been end\n", position->pid);
				position->pid = 0;
				jobs* last;
				jobs* next;
				last = position->last;
				next = position->next;
				/*If the last and next indices both have storages, link the last and next together*/
				if(last->pid!=0 && next->pid!=0 && flag < 4){
					/* put the empty cell to the Most Recent location for next term*/
					jobs* mostRecentNext;
					mostRecentNext = MostRecent->next;
					last->next = next;
					next->last = last;
					position->next = mostRecentNext;
					position->last = MostRecent;
					MostRecent->next = position;
					mostRecentNext->last = position;
				}
			}
		}


		/* Use the status to check if the child process has a valid shell argument */
		if(status == 0 ){ /* only when status == 0, the child run successfully*/
			if(!strcmp(inputBuffer,"exit")){ // If the input has only exit, end child process
				exit(0);
			}
			takenRecord(index, args, inputBuffer, record, RECORD_LIMIT);/*After the command line was taken down, put it into histroy*/
			index++; /*everytime a command line is passed, the index is incremented*/
		}
	}
}

void takenRecord(int index, char* args[],char inputBuffer[], history history[], int limit)
{
	 int ID = index % limit; /* once the index is 10, it will take the position of place 0 -- loop */

		/* the record take down all of the values of its variables*/
		history[ID].index = index;
		memcpy(history[ID].inputBuffer, inputBuffer, MAX_LINE); /* duplicate inputBUffer*/
		positionCalculation( args, history[ID].args, &history[ID].inputBuffer[0]); /* calculate new memory location*/
}

void positionCalculation(char* args[], char* argss[], char* input)
{
		/* since a new char array is created, it is allocated to a new memory location
		*  reassign its location to record[].args*/
		int position = 0;
		int offset = 0;
		do{
			offset += (!position) ? 0 : (args[position] - args[position-1]);
			argss[position] = input + offset;
			position ++;
		}while(args[position] != '\0');
		argss[position] = 0;
}
