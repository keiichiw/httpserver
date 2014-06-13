#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include "server.h"

#define BUFF_SIZE 256

void echoBack(int);

int main (int argc, char* argv[]) {
	int sSock;
	int cSock;
	struct sockaddr_in serverAddr;
	struct sockaddr_in clientAddr;
	int serverPort;

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

		echoBack(cSock);

	}

	return 0;
}

void echoBack(int cSock) {
	char buff[BUFF_SIZE];
	int strsize;
	if ((strsize = read(cSock, buff, BUFF_SIZE)) < 0) {
		perror("recieve Error");
	}
	//printf("%s\n",buff);
	while (strsize > 0) {
		if (write(cSock, buff, strsize) != strsize) {
			fprintf(stderr, "%d/%s\n", strsize, buff);
			perror("send error");
		}

		if ((strsize = read(cSock, buff, BUFF_SIZE)) < 0) {
			perror("recieve Error");
		}
	}
	close(cSock);
	return;
}
