#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include "server.h"

int socket_set (int *sSock, struct sockaddr_in *serverAddr, int portno) {

	if ((*sSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("socket error");
		return -1;
	}

	memset(serverAddr, 0, sizeof(*serverAddr));
	serverAddr->sin_family = AF_INET;
	serverAddr->sin_port = htons(portno);
	serverAddr->sin_addr.s_addr = htonl(INADDR_ANY);


	if (bind(*sSock, (struct sockaddr *) serverAddr, sizeof(*serverAddr)) < 0) {
		perror("bind error");
		return -1;
	}


	if (listen(*sSock, 10) < 0) {
		perror("listen error");
		return -1;
	}
	return 0;
}
