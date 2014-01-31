/**
* Purpose: Solve assignment2 -- Memory Layout Printer
* Author: Yang WU -- yang.wu2@mail.mcgill.ca
* Date: Jan 27th 2014*/

#include <stdio.h>
#include <stdlib.h>

static const int something = 100;
static char* dummy = "blablabla I don't what to say";
static char some[50];
extern char etext, edata, end; /* 	The symbols mus have some type,
									or "gcc -Wall" complians*/
void printfMore();

int main(int argc, char *argv[])
{
	int x;
	int addr[4], size[3]; /* Find all of four points and three sizes in between text, data, bss segments */

	/* Take record of every single point at each end of each segment*/
	addr[0] = (int) &main; /* Here, Assume the address of the main is the starting point of the text segment*/
	addr[1] = (int) &etext;
	addr[2] = (int) &edata;
	addr[3] = (int) &end;

	/* Take record of the sizes of three segments*/
	size[0] = addr[1] - addr[0]; /* An important assumption: three segments are adjacent*/
	size[1] = addr[2] - addr[1];
	size[2] = addr[3] - addr[2];
	printf("First address past:\n");

	printf(" Address of the main func	%x\n", addr[0]);
	printf(" program text (etext)	%x -> %x\n", addr[1], size[0]);
	printf(" initialized data (edata)	%x -> %x\n", addr[2], size[1]);
	printf(" uninitialized data (end)	%x -> %x\n", addr[3], size[2]);


	label2:

	//printfMore();
	printf("location of a label	2	%10p\n", &&label2);

	return 0;
}

void printfMore()
{
	printf(" Address of the printMore func	%10p\n", &printfMore);
	labe3:
	printf("location of labe 3      %10p\n", &&labe3);
}

