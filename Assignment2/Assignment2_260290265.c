/**
* Purpose: Solve assignment2 -- Memory Layout Printer
* Author: Yang WU -- yang.wu2@mail.mcgill.ca
* Date: Jan 27th 2014*/

#include <stdio.h>
#include <stdlib.h>

extern char etext, edata, end; /* 	The symbols mus have some type,
									or "gcc -Wall" complians*/
void printfMore()
{
	printf(" Address of the main func	%10p\n", &printfMore);
}

int main(int argc, char *argv[])
{
	int x;
	label:
	
	printf("First address past:\n");
	printf(" program text (etext)	%10p\n", &etext);	
	printf(" initialized data (edata)	%10p\n", &edata);
	printf(" uninitialized data (end)	%10p\n", &end);
	printf(" Address of the main func	%10p\n", &main);
	printf("location of variable x		%10p\n", &x);
	printf("location of a label		%10p\n", &&label);
	
	label2:
	
	printfMore();
	printf("location of a label	2	%10p\n", &&label2);
	
	return 0;
}


