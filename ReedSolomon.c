#include <stdio.h>
#include <stdlib.h>

/* Assume this decoder is only working for GF(16) */

int GF16_Table[16] = {0,1,2,4,8,3,6,12,11,5,10,7,14,15,13,9};
int input[11] = {1,2,3,4,5,6,7,8,9,10,11};
int GeneratorRoot[4] = {1,2,4,8};
int Generator[5] = {1,15,3,1,12}; /* TODO:turn it to a function based on GeneratorRoot */

#define GFLENGTH 	15
#define SUCCESS		1
#define LEMDA		9 /* In GF(16), the lemda is 9*/
#define DERIVATIVE	14

void AddErrors(int*);
int Division(int, int*, int*, int);
int power(int, int);
int ReedSolomonEncoding(int[], int*, int);
int Location(int);
int GFModulaAdd(int*, int*, int);
int GFMulti(int, int);
int GFMultiplication(int*, int, int*, int,int);
int GFModula(int*, int*, int*, int, int*, int);

int main(){
	int* EncodedCode, fd, loop, size, miniDistance;
	EncodedCode = malloc(GFLENGTH*sizeof(int)); 
	size = GFLENGTH - sizeof(input)/sizeof(int);/*check the length of the generator*/

/*********************The Section of Encoding Algorithm**************************/	
	fd = ReedSolomonEncoding(input, EncodedCode, sizeof(input)/sizeof(int));
	if(!fd)
	{
		fprintf(stderr, "ERROR: Malfunction of Reed Solomon Encoder !!!!");
		return EXIT_FAILURE;
	}
	
	printf(" The sending message is: ");
	for(loop = 0; loop < GFLENGTH; loop++)
	{
		printf(" %d ", EncodedCode[loop]);
	}
	printf("\n");
/*********************The End of Section of Encoding Algorithm*******************/

/*********************The Section of Introducing Errors**************************/
	AddErrors(EncodedCode);
	printf(" The received message is: ");
	for(loop = 0; loop < GFLENGTH; loop++)
	{
		printf(" %d ", EncodedCode[loop]);
	}
	printf("\n");
/*********************The End of Section of Introducing Errors*******************/

/*********************The Section of Syndrome calculation************************/	
	int *syndrome, index;
	syndrome = malloc(size*sizeof(int));
	for(index = 0; index <size ; index++)
	{
		for(loop = 0; loop < GFLENGTH - 1; loop++)
		{
			syndrome[index] ^=  EncodedCode[loop];
			syndrome[index] = GFMulti(syndrome[index], GeneratorRoot[size-index-1]);
		}
		syndrome[index] ^=  EncodedCode[loop];
	}
	printf(" The syndrome polynomials is: ");
	for(loop = 0; loop < size; loop++)
	{
		printf(" %d ", syndrome[loop]);
	}
	printf("\n");
/*********************The End of Section of Syndrome calculation*****************/

/*********************The Section of error location******************************/	
	int *location,*result,*quotient, *factor1, *factor, degree;
	int lemda = LEMDA;
	int diviLemda = Division(1, &lemda,NULL, 0);
	degree = size/2;
	location = malloc((size+1)*sizeof(int));
	result = malloc((size+1)*sizeof(int));
	quotient = malloc(2*sizeof(int));
	location[0] =1;

	fd = GFModula(location, syndrome,result, size, quotient, 1);
	printf(" The first quotient is: ");
	for(loop = 0; loop <2; loop++)
	{
		printf("  %d  ", quotient[loop]);
	}
	printf("\n");
	
	if(fd>(size/2))
	{
		int *divisor, *result1, *quotient1;
		divisor = malloc(fd*sizeof(int));
		result1 = malloc((fd+1)*sizeof(int));
		quotient1 = malloc((size/2)*sizeof(int));
		for(loop=0; loop< fd; loop++) /* refilled the result of last iteration*/
		{
			divisor[loop] = result[size+1-fd+loop];
		}
		fd = GFModula(syndrome, divisor,result1, fd, quotient1, 1);
		printf(" The second quotient is: ");
		for(loop = 0; loop <2; loop++)
		{
			printf("  %d  ", quotient1[loop]);
		}
		printf("\n");
		
		printf("the first factor is:");
		factor = malloc(fd*sizeof(int));
		for(loop =0; loop<fd; loop++)
		{
			
			result1[size-fd+loop] = GFMulti(result1[size-fd+loop], diviLemda);
			factor[loop] = result1[size-fd+loop];
			printf(" %d ", factor[loop]);
		}
		printf("\n");
		
		/* If the iteration appears, degree increments*/
		degree++;
		
		factor1 = malloc(degree*sizeof(int));
		for(loop = 0; loop < size/2; loop++)
		{
			GFMultiplication(quotient, quotient1[loop], factor1, size/2, loop);
		}
		factor1[degree-1] ^= 1;
	}
	else
	{
		memcpy(&factor1, &quotient, (size/2)*sizeof(int));
		printf("the first factor is:");
		factor = malloc(fd*sizeof(int));
		for(loop =0; loop<fd; loop++)
		{
			result[size-fd+loop] = GFMulti(result[size-fd+loop], diviLemda);
			factor[loop] = result[size-fd+loop];
			printf(" %d ", factor[loop]);
		}
		printf("\n");
	}

	printf("the second factor is:");
	for(loop =0; loop<degree; loop++)
	{
		factor1[loop] = GFMulti(factor1[loop], diviLemda);
		printf(" %d ", factor1[loop]);
	}
	printf("\n");
	
	int* correction;
	correction = malloc(GFLENGTH*sizeof(int));
	/* Calculate the locations*/
	for(loop = 0; loop < GFLENGTH; loop++)
	{
		int L,result, amount;
		result = factor1[degree-1];
		amount = GFLENGTH - loop;
		//printf("the position %d has \t", loop);
		for(L=0;L<degree-1;L++)
		{
			//printf(" %d: %d * %d = %d ",L, factor1[L], GF16_Table[1+(2-L)*amount%15], GFMulti(factor1[L], GF16_Table[1+(2-L)*amount%15]));
			result ^= GFMulti(factor1[L], GF16_Table[1+(degree-1-L)*amount%15]);
			
		}
		if(result == 0)
		{
			correction[GFLENGTH-1-loop] = loop;
			printf("At position %d, there is an error\n",loop);
		}
	}

/*********************The End of Section of Error Location***********************/	

/*********************The Section of Error Correction****************************/
	int derivative = DERIVATIVE;
	int dividerivative = Division(1, &derivative,NULL, 0);
	for(loop = 0; loop<GFLENGTH; loop++)
	{
		if(correction[loop])
		{
			int L, amount,result;
			amount = GFLENGTH - correction[loop];
			result = factor[fd-1];
			for(L = 0; L < fd-1; L++)
			{
				result ^= GFMulti(factor[L], GF16_Table[1+(fd-1-L)*amount%15]);
			}
			result = GFMulti(result, GF16_Table[correction[loop]+1]);
			result = GFMulti(result, dividerivative);
			correction[loop] = result;
			//printf("   the result is %d \n", result);
		}
	}
	
	printf("The error is:");
	for(loop = 0; loop < GFLENGTH; loop ++)
	{
		printf(" %d ", correction[loop]);
	}
	printf("\n");
/*********************The End of Section of Error Correction*********************/
	for(loop = 0; loop < GFLENGTH; loop ++)
	{
		if(correction[loop])
		 EncodedCode[loop] ^= correction[loop];
			
	}
	
	printf("The corrected message is:");
	for(loop = 0; loop < GFLENGTH; loop ++)
	{
		printf(" %d ", EncodedCode[loop]);
	}
	printf("\n");
	return EXIT_SUCCESS;
}

int power(int a, int b)
{
	int result, terms;
	terms = b%GFLENGTH;
	while(terms)
	{
		result = GFMulti(result, a);
		terms--;
	}
	return result;
}

void AddErrors(int* msg) /* Later we add some random error at random positions */
{
	msg[5] = msg[5]^13;
	msg[12] = msg[12]^2;
}

int ReedSolomonEncoding(int input[],int* encode, int ArrayLength)
{
	int loop,tmp[GFLENGTH];
	
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
	GFModulaAdd(tmp, encode, (GFLENGTH-ArrayLength));

	return SUCCESS;
}

int GFModulaAdd(int* divident, int* result, int rSize)
{
	int size, term, loop, *tmp;
	term = 0;
	size = sizeof(Generator)/sizeof(int);
	tmp = malloc(size*sizeof(int));
	
	while(term <= (GFLENGTH-size)) /* Do the modulation process*/
	{
		
		GFMultiplication(Generator, divident[term], tmp, size,0);
		//printf("%d %d %d %d %d\n", tmp[0],tmp[1],tmp[2],tmp[3],tmp[4]);
		for(loop =0; loop < size; loop++)
		{
			divident[term+loop] = divident[term+loop]^tmp[loop];
		}
		
		term++;
	}
	for(loop = GFLENGTH-rSize; loop < GFLENGTH; loop++)
	{
		result[loop] = divident[loop];
		//printf(" %d ", result[loop]);
	}
	//printf("\n");
	return SUCCESS;
}

int Division(int divident, int* divisor,int* tmp, int size)
{
	int loop, quotient;
	loop = 0;
	if(divident == 0) 
	{
		quotient = 0;
	}
	else
	{
		for(loop = 1; loop <= GFLENGTH; loop++)
		{
			if(divident == GFMulti(divisor[0], GF16_Table[loop]))
			{
				quotient = GF16_Table[loop];
				break;
			}
		}
	}
	if(tmp == NULL)
		return quotient;
	for(loop=0; loop<size; loop++)
	{
		tmp[loop] = GFMulti(divisor[loop], quotient);
	}
	return quotient;
}

int GFModula(int* divident, int* divisor, int* result, int divisorSize, int* quotient, int offset)
{
	int loop, term, left, *tmp;
	left = 0;
	tmp = malloc(divisorSize*sizeof(int));

	for( term = 0; term <=offset; term++)
	{
		quotient[term] = Division(divident[term], divisor, tmp, divisorSize);
		for(loop =0; loop < divisorSize; loop++)
		{
			divident[term+loop] = divident[term+loop]^tmp[loop];
		}
	}
	for(loop = 0; loop < divisorSize+offset; loop++)
	{
		result[loop] = divident[loop];
		if(divident[loop])
			left++;
		//printf(" %d ", result[loop]);
	}
	return left;
}

int GFMultiplication(int* a, int b, int* c, int size, int offset)
{
	int loop;
	if(b==0)
	{
		for(loop = 0; loop< size; loop ++)
		{
			c[loop] = 0;
		}
	}
	else if(offset)
	{
		for(loop = 0; loop< size; loop ++)
		{
			c[loop+offset] ^= GFMulti(a[loop], b);
		}
	}
	else
	{	
		for(loop = 0; loop< size; loop ++)
		{
			c[loop] = GFMulti(a[loop], b);
		}
	}
	
	return SUCCESS;
}

int GFMulti(int a, int b) /* Used to do the GF multiplication in GF(16)*/
{
	if(a ==0 || b==0)
		return 0;
	int indexA, indexB, index;
	indexA = Location(a);
	indexB = Location(b);
	index = (indexA + indexB)%GFLENGTH + 1;
	//printf("The number right now is %d\n", GF16_Table[index]);
	return GF16_Table[index];
}

int Location(int number)/* Target the index of each number: N = a^m, to get m*/
{
	int loop;
	for(loop = 1; loop <=GFLENGTH; loop++)
	{
		if(number == GF16_Table[loop])
			break;
	}
	return (loop-1);
}
