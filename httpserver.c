#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include "parser.h"
#include "server.h"

#define BUFF_SIZE 256

int echoBack(int);


int main (int argc, char* argv[]) {
	int sSock;
	int cSock;
	struct sockaddr_in serverAddr;
	struct sockaddr_in clientAddr;
	int serverPort;
	pid_t procId;

	if (argc < 2) {
		perror("Please give PortNumber");
		return 1;
	}

	serverPort = atoi(argv[1]);

	if (socket_set(&sSock, &serverAddr, serverPort) < 0) {
		perror("socket set");return 1;
	}


	while (1) {
		unsigned int cAddrSize = sizeof(clientAddr);


		if ((cSock =
				 accept (sSock, (struct sockaddr *) &clientAddr, &cAddrSize)) < 0) {
			perror("accept error");
			return 1;
		}

		if ((procId = fork()) < 0) {
			perror("fork");
		}else if(procId == 0) {
			close(sSock);
			while(1) {
				echoBack(cSock);
			}
			return 0;
		}
		close(cSock);
		int status;
		int r;
		while((r=waitpid(-1, &status,WNOHANG)) != 0){
			if (r < 0) {
				perror("");
			}
		}

	}
	return 0;
}

int echoBack(int cSock) {
	char buff[BUFF_SIZE];
	int strsize;
	if ((strsize = read(cSock, buff, BUFF_SIZE)) < 0) {
		perror("recieve Error");
	}
	reqinfo *r;
	r = (reqinfo*) malloc(sizeof(reqinfo));
	parseMethod(r, buff);

	if (write(cSock, r->host , strlen(r->host)) != (int)strlen(r->host)) {
		fprintf(stderr, "%d/%s\n", strsize, buff);
		perror("send error");
	}

	return strsize;
}
