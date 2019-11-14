#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#define SERVER_ACK		"SERVER ACK\n"
#define BUFFER_LEN		1024

static char *keyArray[1024];
static char *valueArray[1024];


int recvMsg(int fd, char *buff);
void sendMsg(int fd, char *buff, int size);
void error_check(int val, const char *str);
void server(int portNum);
void serveConnection(int clientSocket);
int acceptConnection(int port);
int setupAccept(int port);


int main(int argc, char *argv[]) 
{
	alarm(540);
	if (argc == 1) //no port specified
	{
		printf("No port entered\n");
		exit(0);
	}
	else if (argc == 2 && strcmp(argv[1], "-hw") == 0)
	{
		printf("hello, world\n");
		exit(0);
	}
	else
	{
		//map space for the shared arrays
		for (int n = 0; n < 1024; n++)
		{
			keyArray[n] = mmap(NULL,sizeof(char *), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
			valueArray[n] = mmap(NULL,sizeof(char *), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
		}
		server(strtol(argv[1], NULL, 0));
	}

	return 0;  

}

void server(int portNum)
{
	int rc, serverSocket, clientSocket;
	struct sockaddr_in from;
	int fromlen, gotit;
	int optval = 1;
	int childID;
	serverSocket = setupAccept(portNum);
	while(1)
	{
		clientSocket = accept(serverSocket,(struct sockaddr *)&from,(socklen_t *)&fromlen);
		setsockopt(clientSocket, IPPROTO_TCP, TCP_NODELAY, (char *)&optval, sizeof(optval));
		childID = fork();

		if (childID == 0)
		{
			printf("New Client active on Port: %d\n\n", portNum);
			serveConnection(clientSocket);
		}

		else
		{
			close(clientSocket);
		}
	}

}


void serveConnection(int clientSocket)
{
	int rc, ack_length;
	char buff[1024];
	
	ack_length = strlen(SERVER_ACK)+1;
	rc = recvMsg(clientSocket, buff);
	buff[rc] = '\0';
	while (rc !=0)
	{

		printf("Server received %d bytes :%s: \n", rc, buff);
		sendMsg(clientSocket, (char *)SERVER_ACK, ack_length);
		char *string = strtok(buff, " ");	//grab first token / string
		//compare to put, if user included put, then add key and value to arrays
		if (strcmp(string, "put") == 0) 
		{ 
			//search through our array until we find the first non-NULL array item
			for (int p = 0; p < 1024; p++)
			{
				if (*keyArray[p] == NULL)
				{
					//store initial put into string
					char *keyString = (strtok(NULL, " "));
					memcpy(keyArray[p], strdup(keyString), strlen(strdup(keyString))+1);
					char *valueString = (strtok(NULL, " "));
					memcpy(valueArray[p], strdup(valueString), strlen(strdup(valueString))+1);
					printf("SERVER STORED: %s %s\n\n", keyArray[p], valueArray[p]);
					break;
				}
			}
		} 
		//if user entered get, retrieve the value from arrays
		else if (strcmp(string, "get") == 0)
		{
			char *keyString = strtok(NULL, " ");
			for (int n = 0; n < 1024; n++)
			{
				if (strcmp(keyString, keyArray[n]) == 10)
					printf("SERVER RETRIEVED: %s %s\n\n", keyArray[n], valueArray[n]);
			}
		}
		rc = recvMsg(clientSocket, buff);
		buff[rc] = '\0';
	}
	close(clientSocket);

}

int setupAccept(int port)
{
	int rc, serverSocket;
	int optval = 1;
	struct sockaddr_in serverAddr;

	//create my socket
	//configure settings of the server address struct
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(serverSocket, SOL_SOCKET,SO_REUSEADDR,(char *)&optval,sizeof(optval));
	//bind the address struct to socket
	bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)); 
	//listen on socket, set max connections of 100
	rc = listen(serverSocket, 10);
	error_check(rc, "setupAccept listen");

	return (serverSocket);
}

	
/*Following functions sourced from sockseqsrvr.c file*/

int recvMsg(int fd, char *buff)
{
	int bytesRead;

	bytesRead = read(fd, buff, 1024);
	error_check(bytesRead, "Recv_msg read");
	return(bytesRead);
}


void sendMsg(int fd, char *buff, int size)
{
	int n;

	n = write(fd, buff, size);
	error_check(n, "Send_msg write");
}

void error_check(int val, const char *str)
{
	if (val < 0)
	{
		printf("%s :%d: %s\n", str, val, strerror(errno));
		exit(1);
	}
}


