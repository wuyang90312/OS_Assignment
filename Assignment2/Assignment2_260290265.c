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


static const int something = 100;
static char* str = "The File is Printing Below:\n";
static char some[50];
extern char etext, edata, end; /* 	The symbols mus have some type,
									or "gcc -Wall" complians*/
int checkStack(int);
int checkStack(int index);
void handle_SIGSEGV();

int main(int argc, char *argv[])
{
	struct sigaction sa;

    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = handle_SIGSEGV;
    if (sigaction(SIGSEGV, &sa, NULL) == -1)
    {
		
	}
	
	int addr[5], size[4]; /* Find all of four points and three sizes in between text, data, bss, File Mapping segments */
	int addr_Stk, size_Stk, addr_Hp, size_Hp; /* Define varaibles for addresses and sizes of Stack and Heap */
	int addition = 0xfffffff;
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
	//	Print the	file..	
	int	i;	
	printf("%s", str);
	for(i=0;i<filestat.st_size;	i++)
	{
		putchar(ptr[i]);
	}
	
	munmap(ptr,	filestat.st_size);	
/*****************Text, Data, BSS segments***************************************/	
	printf(" section			Address		size\n");

	//printf(" Address of the main func		0x%x\n", addr[0]);
	printf(" Text segment			0x%x 	%x\n", addr[0], size[0]);
	printf(" initialized data segment	0x%x 	%x\n", addr[1], size[1]);
	printf(" uninitialized data segment	0x%x	%x\n", addr[2], size[2]);
	printf(" file mapping segment		0x%x	%x\n", addr[4], size[3]);
/*****************Heap and Stack segments***************************************/	
	/*Get the starting point of the Stack*/
	/*i = 0;
	addr_Stk = (int) alloca(0);
	printf("the starting point is %x  - %c\n", addr_Stk, &addr_Stk);
	printf("the output  is %x\n", alloca(0xfffff));
	//printf("the starting point is %x\n", alloca(0xffffe));
	while(!checkStack(i)){
		i+=16;
		//printf("run\n");
		printf("the size is %x\n", i);
	}
	size_Stk = i;
	printf("the size is %x\n", size_Stk);*/

	i = 0;
	addr_Hp = (int) malloc(0);
	printf("the starting point is %x  - %c\n", addr_Hp, &addr_Hp);
	while(addition != 1){
		if(!checkHeap(i))
		{
			i -= addition;
			addition = addition /2;
		}
		i+= addition;
	}
	size_Hp = i;
	printf(" Heap Sement			0x%x	%x\n", addr_Hp, size_Hp);
	free(addr_Hp);
/*******************************************************************************/	

	return 0;
}

int checkStack(int index)
{
	int addr = alloca(index);
	//printf("the output  is %x\n", alloca(index));
	if(!alloca(0xfffffffff))
	{
		printf("hahaha\n");
		return 1;
	}
	else
		return 0;

}

int checkHeap(int index)
{
	int* addr = (int*) malloc(index);
	free(addr);
	return addr;
		
}

void handle_SIGSEGV()
{
	fprintf( stderr, "MEM is OUT\n");
}
