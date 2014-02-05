/**
* Purpose: Solve assignment2 -- Memory Layout Printer
* Author: Yang WU -- yang.wu2@mail.mcgill.ca
* Date: Jan 27th 2014*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>	
#include <sys/types.h>	 
#include <sys/stat.h>	
#include <fcntl.h>	
#include <alloca.h>
#include <signal.h>

static int *glob_var; /*create a global variable*/
static const int something = 100;
static char* str = "The File is Printing Below:\n";
static char some[50];
extern char etext, edata, end; /* 	The symbols mus have some type,
									or "gcc -Wall" complians*/
int checkHeap(int index);

int main(int argc, char *argv[])
{
/*********************************************************************************************************/
	pid_t pid;
	int addr[5], size[4]; /* Find all of four points and three sizes in between text, data, bss, File Mapping segments */
	int addr_Stk, size_Stk, addr_Hp, size_Hp; /* Define varaibles for addresses and sizes of Stack and Heap */
	int addition = 0x10000000; /* set up the addition value to a large number*/

/*****************Text, Data, BSS segments***************************************/	
	/* Take record of every single point at each end of each segment*/
	addr[0] = (int) &main; /* Here, Assume the address of the main is the starting point of the text segment*/
	addr[1] = (int) &etext;
	addr[2] = (int) &edata;
	addr[3] = (int) &end;

	/* Take record of the sizes of three segments*/
	size[0] = addr[1] - addr[0]; /* An important assumption: three segments are adjacent*/
	size[1] = addr[2] - addr[1];
	size[2] = addr[3] - addr[2];
	
/*************************** Process the file, get the File Mapping Segment ************************/
	if(argc != 2)	
	{	
		printf("\nUsage:	%s	file-­‐to-­‐cat\n",argv[0]);
		exit(EXIT_FAILURE);	
	}

	//Open the file...	
	int	fd = open(argv[1], O_RDONLY);	
	  
	if(fd < 0)	
	{
		perror("Opening file");	
		exit(EXIT_FAILURE);	
	}
		
	//Get the size of the file	
	struct stat filestat;	
	if(fstat(fd,&filestat) == -1)	
		perror("Unable to get file size");
	
	char *ptr =	(char*)mmap(NULL, filestat.st_size,	PROT_READ, MAP_PRIVATE,	fd,	0);		
	addr[4] = (int) ptr;
	size[3] = filestat.st_size;
	
	munmap(ptr,	filestat.st_size);	
/*****************Heap and Stack segments***************************************/	
	/*Get the starting point of the Stack*/
	int	i = 0;
	addr_Hp = (int) malloc(0);
	while(addition != 1){ /* once if addition reaches 1, stop while loop */
		if(!checkHeap(i)) /* once malloc overflows, reduce the addition value by a factor of 2 */
		{
			i -= addition; /* restore the value before overflow*/
			addition = addition /2;
		}
		i+= addition;
	}
	size_Hp = i;
	free(addr_Hp);
	
	glob_var = mmap(NULL, sizeof *glob_var, PROT_READ | PROT_WRITE, 
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0); /* create a global varibles between two processes*/
    *glob_var = 1;
    i = 0;
	addr_Stk = (int) alloca(0);
	
	printf("Process ");
	pid = fork();
	if(pid == 0){
		char* addr_stk = NULL;
		while(1){
			addr_stk = alloca(1);
			i++;
			*glob_var = i;
			printf(".");
		}
		exit(EXIT_SUCCESS);
	}
	else
	{
		wait(NULL);
		size_Stk = *glob_var-1;
		munmap(glob_var, sizeof *glob_var);
	}
/*****************Print Text, Data, BSS, and mapping segments*******************/	
	printf("\n section			starting Address	size\n");
	printf(" Text segment			0x%x 		%x\n", addr[0], size[0]);
	printf(" initialized data segment	0x%x 		%x\n", addr[1], size[1]);
	printf(" uninitialized data segment	0x%x		%x\n", addr[2], size[2]);
	printf(" file mapping segment		0x%x		%x\n", addr[4], size[3]);
	printf(" Heap Segment			0x%x		%x\n", addr_Hp, size_Hp);
	printf(" Stack Segment			0x%x		%x\n", addr_Stk, size_Stk);
/*******************************************************************************/	

	exit(EXIT_SUCCESS);
}

int checkHeap(int index) /* Check if the Heap is overflowed */
{
	int* addr = (int*) malloc(index);
	free(addr);
	return addr; /* if malloc overflows, return 0 */
		
}
