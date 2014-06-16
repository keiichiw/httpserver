#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "server.h"
#include "parser.h"
#include "header.h"
#define BUFF_SIZE 512
#define MESSAGE_SIZE 10240
char last4[4];
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
		return "501 Not Implemented";
	}
}

int isLastLine (char* s) { //line ends with \r\n\r\n
	int len = strlen(s);
	if (len >= 4 && strcmp(s+len-4, "\r\n\r\n") == 0) {
		return 1;
	}
	return 0;
}
void strshift(char* str, int df) {
	int i =0;
	for (i = df; i<strlen(str);i++) {
		str[i-df] = str[i];
	}
	return;
}
reqinfo* getRequest (int cSock) {
	char buff[MESSAGE_SIZE];
	char* top;
	int strsize = 0, tsize;
	reqinfo *r;
	r = (reqinfo*) malloc(sizeof(reqinfo));
	r -> host = "";
	r -> status = 200;
	//Read Request

	while ((tsize = read(cSock, buff+strsize, MESSAGE_SIZE - strsize)) >= 0) {
		if (tsize > 0) {
			strsize += tsize;
		}
		buff[strsize] = '\0';
		if (isLastLine(buff)) break;
		if (strsize >= MESSAGE_SIZE) {
			perror("Request is too long");
			fprintf(stderr, "size=%d\n %s", strsize, buff);
			r -> status = 400;
			return r;
		}
	}

	//Parse Request
	buff[strsize] = '\0';
	chomp(buff);

	fprintf(stderr, "\n+++++1++++\n%s\n+++2++\n", buff);



	top = parseMethod(r, buff);
	//parseHeader(r, top);

	//check Host:
	/*
	if (strlen(r -> host) == 0) {
		if (r -> version != 0) {
			fprintf(stderr, "Error No Host HTTP/%d %d", r->version, r -> error);
			r -> status = 400;
		}
	}
	*/
  return r;
}

int sendHeadRes (reqinfo* r, int cSock, int flg) {
	char res[100];
	char time[100];
	getGMT(time);
	sprintf (res,
					 "HTTP/1.%d %s\r\nDate: %s\r\n\r\n",
					 r -> version, statusMessage(r),
					 time);

	fprintf(stderr, "%s\n", res);
	if (write(cSock, res, strlen(res)) != strlen(res)) {
		perror("Fail to send message");
		return -1;
	}
	if (flg == 1) close(cSock);
	return 0;
}


void sendGetRes (reqinfo* r, int cSock) {
	char path[BUFF_SIZE];
	char buff[BUFF_SIZE];
	int fd;
	int strcnt = 0;
	sprintf(path, "%s%s", r -> root, r -> uri);
	sendHeadRes (r, cSock, 0);

	fprintf(stderr, "Path:%s", path);
	if ((fd = open(path, O_RDONLY)) == -1) {
		r -> status = 400;
		if (errno == EACCES) {
			r -> status = 404;
		}
		fprintf(stderr, "%s don't exist.", path);
		close(fd);
		return;
	}
	int sumcnt = 0;
	fprintf(stderr,"sum = %d",sumcnt);
	while ((strcnt = read(fd, buff, BUFF_SIZE)) != 0) {
		sumcnt += strcnt;
		fprintf(stderr,"sum = %d",sumcnt);
		int wcnt = 0;
		int flg  = 0;
		if (strcnt == -1) {
			r -> status = 404;
			break;
		}
		while (wcnt < strcnt &&
					 (flg  = write (cSock, buff + wcnt, strcnt - wcnt))) {
			if (flg == -1) {
				perror("Fail to send message");
			}
			wcnt += flg;
			fprintf(stderr,"wcnt = %d",wcnt);
		}
	}

	close(fd);
	close(cSock);
	perror("connection closed");
	return;
}


void sendResponse (reqinfo* r, int cSock) {
	printReq(r);
	if (r -> method == 0 && r -> status == 200) {
		sendGetRes(r, cSock);
	} else {
		perror("ERROR");
		sendHeadRes(r, cSock, 1);
	}
	return;
}
