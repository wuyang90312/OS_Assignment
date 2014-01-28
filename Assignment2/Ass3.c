#include <stdio.h>	
#include <stdlib.h>	
#include <sys/mman.h>	
#include <sys/types.h>	 
#include <sys/stat.h>	
#include <fcntl.h>	
  
	
  
int main(int argc, char	*argv[])	
{	printf("hello1\n");
 
	if(argc != 2)	
	{	
		printf("\nUsage:	%s	file-­‐to-­‐cat\n",argv[0]);
		exit(EXIT_FAILURE);	
	}	
  printf("hello2\n");

  
	//Open the file...	
	int	fd = open(argv[1], O_RDONLY);	
	  
	if(fd < 0)	
	{
		perror("Opening file");	
		exit(EXIT_FAILURE);	
	}	
  printf("hello3\n");
	//Get the size of the file	
	struct stat filestat;	
	if(fstat(fd,&filestat) == -1)	
		perror("Unable to get file size");	
	  
	//	Do	a	memory	map	of	the	file.	
	char *ptr =	(char*)mmap(NULL, filestat.st_size,	PROT_READ, MAP_PRIVATE,	fd,	0);	
printf("hello4\n");
	//	Print the	file..	
	int	i;	

	for(i=0;i<filestat.st_size;	i++)
	{
		putchar(ptr[i]);
	}	
printf("hello5\n");   
	//	Unmap	the	memory	region.	
	munmap(ptr,	filestat.st_size);	
	exit(EXIT_SUCCESS);	
  
}	
	
  
