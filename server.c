#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include "server.h"
#include "parser.h"
#define BUFF_SIZE 256


reqinfo* getRequest (int cSock, char* droot) {
	char buff[BUFF_SIZE];
	int strsize;
	reqinfo *r;
	r = (reqinfo*) malloc(sizeof(reqinfo));
	r -> host = "";
	if ((strsize = read(cSock, buff, BUFF_SIZE)) < 0) {
		perror("recieve Error");
	}
	buff[strsize] = '\0';
	fprintf(stderr, "%s", buff);
	parseMethod(r, buff);


	if (r -> error) {
		fprintf(stderr, "END :%d\n", r->error);
	}

	while ((strsize = read(cSock, buff, BUFF_SIZE)) < 0) {
		if(strlen(buff) <= 2) {
			break;
		}
		fprintf(stderr, "%s", buff);
		parseHeader(r, buff);
	}

	if (r -> error) {
		fprintf(stderr, "END :%d\n", r->error);
	}

	if (strlen(r -> host) == 0) {
		if (r -> version == 0) {
			r -> host = droot;
		} else {
			fprintf(stderr, "No Host HTTP/%d %d", r->version, r -> error);
		}
	}

  return r;
}
void sendGetRes (reqinfo* r, int cSock) {
	char line1[100] = "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n";
	char line2[100] = "<html><head></head><body>hello.</body></html>\n";
	if (write(cSock, line1, strlen(line1)) != strlen(line1)) {
		perror("Fail to send message");
	}
	if (write(cSock, line2, strlen(line2)) != strlen(line2)) {
		perror("Fail to send message");
	}
	close(cSock);
	return;
}
void sendHeadRes (reqinfo* r, int cSock) {

}

void sendResponse (reqinfo* r, int cSock) {
	if (r -> method == 0) {
		sendGetRes(r, cSock);
	} else if (r -> method == 1) {
		sendHeadRes(r, cSock);
	} else {
		perror ("No Method");
	}
	return;
}
