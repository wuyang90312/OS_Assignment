#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>	
#include <sys/types.h>	 
#include <sys/stat.h>
#include <sys/wait.h>	
#include <fcntl.h>	
#include <alloca.h> 
#include <signal.h>


extern char  etext, edata, end; 
static int *start_t, *start_m, *sizeStack, *sizeHeap, *pointer, *mpointer, *end_m, *end_t;

int main (int argc, char *argv[] ){

	int status, mstatus;
	/*
	Only one file a can be input per time
	*/
	if(argc != 2){
		printf("ERROR!!\nPlease make sure the input command is correct!!\n");
		exit(0);
	}

	/*
	Open the input file in Read Only Mode
	*/
	int fd = open(argv[1], O_RDONLY);
	if(fd < 0){
		printf("ERROR!!\nFail to Open the input file!!\n");
		exit(0);
	}


	/*
	Get the size of the file
	*/

	struct stat filestat;
	if	(fstat(fd, &filestat) == -1){
		printf("Unable to get file size\n");
	}

	char *ptr =	(char*)mmap(NULL, filestat.st_size,	PROT_READ, MAP_PRIVATE,	fd,	0);		
	int fileAdd, fileSize;
	fileAdd = (int) ptr;
	fileSize = filestat.st_size;
	munmap(ptr,	filestat.st_size);	
	
	size_t a, b;
	pid_t pid, hid, mid;
	// int *pointer; 
	// int *mpointer;
	int count, mcount;
	count = 0;
	mcount = 0;
	// pointer = mmap(NULL, sizeof *pointer, PROT_READ | PROT_WRITE, 
 //                    MAP_SHARED | MAP_ANONYMOUS, -1, 0); 
 //    mpointer = mmap(NULL, sizeof *mpointer, PROT_READ | PROT_WRITE, 
 //                    MAP_SHARED | MAP_ANONYMOUS, -1, 0); 	
	start_t = mmap(NULL, sizeof *start_t, PROT_READ | PROT_WRITE, 
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0); 
    sizeHeap = mmap(NULL, sizeof *sizeHeap, PROT_READ | PROT_WRITE, 
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    sizeStack = mmap(NULL, sizeof *sizeStack, PROT_READ | PROT_WRITE, 
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    start_m = mmap(NULL, sizeof *start_m, PROT_READ | PROT_WRITE, 
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    end_m = mmap(NULL, sizeof *end_m, PROT_READ | PROT_WRITE, 
    				MAP_SHARED | MAP_ANONYMOUS, 0, 0);
    end_t = mmap(NULL, sizeof *end_t, PROT_READ | PROT_WRITE, 
    				MAP_SHARED | MAP_ANONYMOUS, 0, 0);
 

	pid = fork();

	

	/*
	Use different child to handle error that will produce by 
	overflowing the heap, stack and mmap.
	*/

	pointer = alloca(0);
	start_t = pointer;
	end_t = pointer;
	if(pid == 0){
		/*
		Test on Stack
		*/
		printf("Loading stuff into the Stack...........\nIt will take a while");
		a = 1;
		while(1){
			end_t = pointer;
			*sizeStack = count;
			pointer = alloca(a);
			if(count%20000 == 0){
				printf(".");
			}
			count++;
		}
		exit(0);
	}else{
		wait(&status);
		mpointer = malloc(0);
		start_m = mpointer;
		end_m = mpointer;
		hid = fork();
		if(hid == 0){
			b = 1;
			printf("\nLoading stuff into the Heap............\n");
			while(1){
				end_m = mpointer;
				*sizeHeap = mcount;
				mpointer = malloc(b);
				if(mcount%1000000 == 0){
					printf(".");
				}
				mcount++;				
				if(mpointer == NULL){
					break;
				}
			}
			exit(0);
		}else{
			wait(&mstatus);
			
		}

	}
	printf("\nStack start address: 			%p\n",start_t);
	//printf("Stack end address: 			%p\n",end_t);
	printf("Stack size:			  	    %d bytes\n",*sizeStack);
	printf("Heap start address: 			 %p\n",start_m );
	//printf("Heap end address: 			 %p\n",end_m);
	printf("Heap size: 			 	 %d bytes\n",*sizeHeap);
	printf("program text segment(etext)     	%10p\n", &etext);
    printf("initialized data segment(edata) 	%10p\n", &edata);
    printf("uninitialized data segment (end)	%10p\n", &end);
    printf("file maping address:   			0x%x\n", fileAdd);
    printf("file size: 				      %d bytes\n", fileSize);
	exit(0);
}

