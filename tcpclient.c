#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int main(int argc, char* argv[]) {
	int sock;
	struct sockaddr_in serverAddr;
	char sendbuf, getbuf;;
	char *host;
	int serverPort;

	if (argc < 3) {
		perror("Please give Host and PortNumber");
		return 1;
	}

	host = argv[1];
	serverPort = atoi(argv[2]);

	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("socket error");
		return 1;
	}

	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(host);
	serverAddr.sin_port = htons(serverPort);

	int t;
	if ((t=connect(sock, (struct sockaddr *) &serverAddr,
								 sizeof(serverAddr))) < 0) {
		printf("%d\n",errno);
		perror("connection error");
		return 1;
	}

	while((sendbuf=getchar()) != EOF) {


		if (write(sock, &sendbuf, 1) != 1) {
			perror("send error");
			break;
		}


		if (read(sock, &getbuf, 1) != 1) {
			perror("recv error");
			return 1;
		}

		printf("%c", getbuf);

	}

	close(sock);
	return 0;
}
