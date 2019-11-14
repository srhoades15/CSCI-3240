#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


int main(int argc,char *argv[], char *envp[])
{
   int  pipeFD[2], i;
   pid_t childID;
   FILE *openFile;
  
  //integer value created to generate char array of args
  //args will be stored in array, sent to execve as the
  //flags used by sort 
   i = argc;
   char *args[i];

   //first arg is set to file type
   //if argc is greater than or equal to 3,set flags as elements 1, 2 
   //in our args array
   args[0] = "usr/bin/sort";
   args[i-1] = NULL;
   if (argc >= 3)
   {
	   args[1] = argv[2];
   }
   if (argc == 4)
   {
	   args[2] = argv[3];
   }
   if (argc == 5)
   {
	   args[3] = argv[4];
   }
   //if only 1 arg,./p4 was entered, then return 0
   if (argc == 1)
   {
	   return 0;
   }
   //if user entered -hw, the print hello, world
   else if (argc == 2 && strcmp(argv[1], "-hw") == 0)
   {
	   printf("hello, world\n");
   }
   //if user entered correct number of args
   //begin our pipe / fork process
   else
   {
	//create our pipe
	pipe(pipeFD);
	//create our fork
	childID = fork();
	
	//begin processesj
	if (childID == 0)
	{
		//in child
		alarm(60);
		//set end of pipe to std in
		close(0);
		dup2(pipeFD[0], 0);
		//close unused pipes
		close(pipeFD[0]);
		close(pipeFD[1]);
		//run sort through execve
		execve("/usr/bin/sort", args,envp);
	        exit(0);	
 	}
	else
	{
		//in parent
		alarm(60);
		//establish a char * to array to feed through pipe
		char *buff[100];
		char line[128];
		//open and read from the file, saving the total number of lines
		//to set a null value, and later print
		openFile = fopen(argv[1], "r");
		if (!openFile)
		{
			printf("file not found\n");
			return 0;
		}
		int n = 0;
		while(fgets(line, sizeof(line), openFile)!=NULL)
		{
			//if line does not include "foobar", then store in array
			if (strstr(line, "foobar") == NULL)
			{
				buff[n] = strdup(line);
				n++;
			}
		}
		buff[n] = NULL;
		dup2(pipeFD[1], 1);
		close(pipeFD[1]); 
		close(pipeFD[0]);
		
		for (int j = 0; j < n; j++)
		{
			printf("%s", buff[j]);
		}
		
	}
   }
   return 0;
}	
