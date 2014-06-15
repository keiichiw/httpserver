#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include "server.h"
#include "parser.h"
#define BUFF_SIZE 2048

char* statusMessage (reqinfo* r) {
	switch (r -> status) {
	case 200:
		return "200 OK";
	case 400:
		return "400 Bad Request";
	case 403:
		return "403 Forbidden";
	case 404:
		return "404 Not Found";
	default:
		return "404 Not Found";
	}
}

void printError (reqinfo* r) {
	if (r -> error) {
		fprintf(stderr, "ERROR :%d\n", r->error);
	}
}

int isLastLine (char* s) { //line ends with \r\n\r\n
	int len = strlen(s);
	if (len > 2 && s[len-1] == '\n'
			&& s[len-2] == '\r' && s[len-3] == '\n') {
			return 1;
	}
	return 0;
}

reqinfo* getRequest (int cSock) {
	char buff[BUFF_SIZE];
	char* top;
	int strsize = 0, tsize;
	reqinfo *r;
	r = (reqinfo*) malloc(sizeof(reqinfo));
	r -> host = "";

	//Read Request
	while ((tsize = read(cSock, buff+strsize, BUFF_SIZE - strsize)) > 0) {
		strsize += tsize;
		if (isLastLine(buff)) break;
	}
	if (strsize >= BUFF_SIZE || isLastLine (buff) == 0) {
		perror("Request is too long");
		r -> status = 400;
		return r;
	}

	//Parse Request
	buff[strsize] = '\0';
	chomp(buff);
	top = parseMethod(r, buff);
	parseHeader(r, top);

	//check Host:
	if (strlen(r -> host) == 0) {
		if (r -> version != 0) {
			fprintf(stderr, "Error No Host HTTP/%d %d", r->version, r -> error);
			r -> status = 400;
		}
	}

  return r;
}

int sendHeadRes (reqinfo* r, int cSock) {
	char res[100];
	sprintf (res,
					 "HTTP/1.%d %s Content-type: text/html\r\n\r\n",
					 r -> version, statusMessage(r));

	if (write(cSock, res, strlen(res)) != strlen(res)) {
		perror("Fail to send message");
		return -1;
	}
	return 0;
}

//TODO at first
void sendGetRes (reqinfo* r, int cSock) {
	char line2[100] = "<html><head></head><body>hello.</body></html>\n";

	sendHeadRes (r, cSock);

	if (write(cSock, line2, strlen(line2)) != strlen(line2)) {
		perror("Fail to send message");
	}
	close(cSock);
	return;
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
