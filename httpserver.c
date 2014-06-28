#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include "sock.h"
#include "server.h"

#define BUFF_SIZE 256


int main (int argc, char* argv[]) {
	int sSock;
	int cSock;
	struct sockaddr_in serverAddr;
	struct sockaddr_in clientAddr;
	char* droot;
	int serverPort;
	pid_t procId;

	if (argc < 3) {
		puts("Usage: ./httpserver <PortNumber> <Root directory>");
		return 1;
	}

	serverPort = atoi(argv[1]);
	droot = argv[2];

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
			reqinfo r;
			r.status = 200;
			r.error = 0;
			getRequest(&r, cSock);
			if (r.status == 200 ) {
				strcpy(r.root, droot);
			}
			sendResponse (&r, cSock);
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
