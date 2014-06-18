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
#define BUFF_SIZE 1024
#define MESSAGE_SIZE 1024
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
	for (i = df; i < (int) strlen(str);i++) {
		str[i-df] = str[i];
	}
	return;
}
void getRequest (reqinfo* r, int cSock) {
	char buff[MESSAGE_SIZE];
	int strsize = 0, tsize;

	//Read Request

	while ((tsize = read(cSock, buff+strsize, MESSAGE_SIZE - strsize)) >= 0) {
		if (tsize > 0) {
			strsize += tsize;
		}
		buff[strsize] = '\0';
		if (isLastLine(buff)) break;
		if (strsize >= MESSAGE_SIZE) {
			perror("Request is too long");
			r -> status = 400;
			return;
		}
	}
	//Parse Request
	buff[strsize] = '\0';
	chomp(buff);

	parseMethod(r, buff);
	fprintf(stderr, "%s %d %d\n", r->uri, r->status, r->error);
	//parseHeader(r, top);
  return;
}

int sendHeadRes (reqinfo* r, int cSock, int flg) {
	char res[100];
	char time[100];
	int cnt = 0;
	int sum = 0;
	getGMT(time);
	sprintf (res,
					 "HTTP/1.%d %s\r\nDate: %s\r\n\r\n",
					 r -> version, statusMessage(r),
					 time);

	while ((cnt = write(cSock, res + sum, strlen(res) - sum)) > 0) {
		sum += cnt;
		if (sum == (int) strlen(res)) break;
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

	fprintf(stderr, "Path:%s", path);
	if ((fd = open(path, O_RDONLY)) == -1) {
		r -> status = 404;
		fprintf(stderr, "%s don't exist.", path);
		sendHeadRes (r, cSock, 1);
		return;
	}

	sendHeadRes (r, cSock, 0);

	int sumcnt = 0;

	while ((strcnt = read(fd, buff, BUFF_SIZE)) != 0) {
		if (strcnt > 0)sumcnt += strcnt;
		int wcnt = 0;
		int flg  = 0;
		if (strcnt == -1) {
			r -> status = 404;
			continue;
		}
		while (wcnt < strcnt &&
					 (flg  = write (cSock, buff+wcnt, strcnt - wcnt))) {
			if (flg == -1) {
				perror("Fail to send message");
				break;
			}
			wcnt += flg;
		}
	}
	close(fd);
	close(cSock);
	perror("connection closed");
	return;
}


void sendResponse (reqinfo* r, int cSock) {
	fprintf(stderr, "hoge%s %d %d\n", r->uri, r->status, r->error);
	//printReq(r);
	if (r -> method == 0 && r -> status == 200) {
		sendGetRes(r, cSock);
	} else {
		sendHeadRes(r, cSock, 1);
	}
	return;
}
