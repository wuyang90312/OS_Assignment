#include <stdio.h>
#include <stdlib.h>

/* Assume this decoder is only working for GF(16) */

int GF16_Table[16] = {0,1,2,4,8,3,6,12,11,5,10,7,14,15,13,9};
int input[11] = {1,2,3,4,5,6,7,8,9,10,11};
int Generator[5] = {1,15,3,1,12};

#define GFLENGTH 	15
#define SUCCESS		1

int ReedSolomonEncoding(int[], int*, int);
int Location(int);
int GFModula(int*, int*);
int GFMulti(int, int);
int GFMultiplication(int*, int, int);

int main(){
	int* EncodedCode, fd, loop;
	EncodedCode = malloc(GFLENGTH*sizeof(int)); 
	
	fd = ReedSolomonEncoding(input, EncodedCode, sizeof(input)/sizeof(int));
	if(!fd)
	{
		fprintf(stderr, "ERROR: Malfunction of Reed Solomon Encoder !!!!");
		return EXIT_FAILURE;
	}
	
	for(loop = 0; loop < GFLENGTH; loop++)
	{
		printf(" %d ", EncodedCode[loop]);
	}
	printf("\n");
	
	return EXIT_SUCCESS;
}

int ReedSolomonEncoding(int input[],int* encode, int ArrayLength)
{
	int loop, reminder[GFLENGTH-ArrayLength],tmp[GFLENGTH];
	printf("The length is %d\n", sizeof(reminder)/sizeof(int));
	
	for(loop = 0; loop < GFLENGTH; loop++)
	{
		if(loop < ArrayLength)
		{
			encode[loop] = input[loop];
			tmp[loop] = input[loop];
		}
		else
		{
			encode[loop] = 0;
			tmp[loop] = 0;
		}
	}
	GFModula(tmp, reminder);
	
	
	return SUCCESS;
}

int GFModula(int* divident, int* result)
{
	int size, term;
	term = 0;
	size = sizeof(Generator)/sizeof(int);
	printf("The length is %d", size);
	
	//while(term < 9)
	//{
		GFMultiplication(Generator, divident[0],sizeof(Generator)/sizeof(int));
		//term++;
	//}
}

int GFMultiplication(int* a, int b, int size)
{
	int loop;
	for(loop = 0; loop< size; loop ++)
	{
		a[loop] = GFMulti(a[loop], b);
	}
	return SUCCESS;
}

int GFMulti(int a, int b)
{
	int indexA, indexB, index;
	indexA = Location(a);
	indexB = Location(b);
	index = (indexA + indexB)%GFLENGTH + 1;
	printf("The number right now is %d\n", GF16_Table[index]);
	return GF16_Table[index];
}

int Location(int number)
{
	int loop;
	for(loop = 1; loop <=GFLENGTH; loop++)
	{
		if(number == GF16_Table[loop])
			break;
	}
	return (loop-1);
}
