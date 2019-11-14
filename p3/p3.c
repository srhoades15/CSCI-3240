#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <pthread.h>

typedef struct rangeValues{
	int firstVal;
	int secondVal;
	int primes;
} rangeValues;
typedef struct rangeValues* rangePtr;
struct rangeValues rangeArray[4];
struct rangeValues *rangeArrayPtr;

void* check_prime(void *);
double timer();
void alarm();

int main(int argc, char **argv)
{
	
	int r,rc, primeCount = 0, numPrime = 0;
	rangeArrayPtr = rangeArray;
	void *arrayPtr = NULL;
	double t1, t2;
	void *finalPrime;
	pthread_t threadIDs[(argc - 1) / 2];
	

	//set alarm to kill program after 90 seconds
	alarm(90);
	//set timer to begin tracking time spent running program
	//time stored in double t1
	t1 = timer();

	//evaluate our arguments using the number of arguments entered
	//if only 1 argument entered, the file name, then return output to user
	//will later change to return only 0
	if (argc == 1)
	{
		return 0;
	}
	//evaluate if the user entered to many arguments. Our program will take only
	//4 ranges, so if more than 9 args are entered, then we mark invalid
	//later update to return only 0
	else if (argc > 9)
	{
		printf("too many items entered\n");
		return 0;	
	}
	//if our args are within the sought after range, OK to proceed
	else	
	{
		//if the 2nd arg is "-hw", then we print out "hello, world"
		if (strcmp(argv[1], "-hw") == 0 && argv[2] == NULL)
		{
			printf("hello, world\n");
			return 0;
		}
		//if 2nd arg is empty, then we output that no items were entered
		//later update to return 0;
		else if (argv[1] == NULL)
		{
			return 0;
		}
		//user enters range(s) to evaluate, start storing into the array pointed to by
		//our rangeArrayPtr
		else
		{
			for (int i = 1; i <= argc; i++)
			{
				if (argv[i] != NULL)
				{
					if (i % 2 != 0)
					{
						rangeArrayPtr[primeCount].firstVal = atoi(argv[i]);
						rangeArrayPtr[primeCount].primes = 0;
					}
					else
					{
						rangeArrayPtr[primeCount].secondVal = atoi(argv[i]);
						//when we get to 2nd value in range, if we aren't at the last argument
						//then we will increment our primeCount
						if (i < argc)
							primeCount++;
						
					}
				}
			}
	
		}
	}
	//begin our individual threads for each range pointer, and check for the number of primes
	for (int i = 0; i < primeCount; i++)
	{
		rc = pthread_create(&threadIDs[i], NULL, check_prime, &rangeArrayPtr[i]);
	}
	//end our threads
	for (int k = 0; k < primeCount; k++)
	{
		pthread_join(threadIDs[k], NULL);
	}
	//count our total number of primes, to be included in output
	for (int j = 0; j < primeCount; j++)
	{
		numPrime += rangeArrayPtr[j].primes;
	}
	//establish a 2nd timestamp
	t2 = timer();

	//print out put of our results and time it took to complete program
	printf("%lf   %d   ", t2-t1, numPrime);
	for (int m = 0; m < primeCount; m++)
	{
		if (m == primeCount -1)
			printf("%d\n", rangeArrayPtr[m].primes);
		else
			printf("%d   ", rangeArrayPtr[m].primes);
	}
	return 0;
}	



void* check_prime(void *value)
{
	int i, j, primeCount = 0, prime = 0, r;
	rangeValues *newRangePtr = (struct rangeValues*) malloc(sizeof(struct rangeValues));

	newRangePtr = value;
	
	for (i = newRangePtr->firstVal; i <= newRangePtr->secondVal; i++)
	{
		for (j = 2, r = sqrt((double) i); (i % j) != 0 && j < r; j++)
		;
		if (i % j != 0)
		{
			primeCount++;

		}
	}
	
	newRangePtr->primes = primeCount++;
	return 0;
}


double timer()
{
	struct timeval t1;

	gettimeofday(&t1, (struct timezone *) 0);
	return ((double) (t1.tv_sec + (t1.tv_usec / 1000000.0)) );
}
