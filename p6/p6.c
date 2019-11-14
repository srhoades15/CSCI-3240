#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

int sigTriggers;
void sigHandler(int sigNum)
{         
	printf("received ");
	if (sigNum == SIGUSR1)
	{
		printf("SIGUSR1\n");
       		sigTriggers++;   
	}
	else if (sigNum == SIGUSR2)    
	{
		printf("SIGUSR2\n");
		exit(0);
	}	
}



int main(int argc, char *argv[])
{
	
	if (argc == 2 && strcmp(argv[1], "-hw") == 0)
	{
		printf("hello, world\n");
		exit(0);
	}
	else
	{
		alarm(90);
		sigTriggers = 0;
		for (;;)
		{
			sleep(1);
			if (sigTriggers < 4)
			{
				signal(SIGUSR1, sigHandler);
				signal(SIGUSR2, sigHandler);
			}
			if (sigTriggers == 4)
			{
				printf("terminating due to 4 SIGUSR1 signals\n");
				exit(0);
			}
		}
		
	}
	return 0;
}



