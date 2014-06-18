#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#define BUFF_SIZE 1024


void printReq (reqinfo* r) { //for debug
	fprintf(stderr, "PrintReq----------\n");
	fprintf(stderr, "Root:%s\n", r -> root);
	fprintf(stderr, "Debug num:%d\n", r -> error);
	fprintf(stderr, "Method:%s\n", (r -> method == 0)?"GET":"HEAD");
	fprintf(stderr, "Status: %d\n", r -> status);
	fprintf(stderr, "URI:%s\n", r -> uri);
	fprintf(stderr, "HTTP/1.%d\n", r -> version);
	fprintf(stderr, "------------------\n");
}


//remove CR-LF
void chomp (char* c) {
	int len = strlen(c);
	if (len >= 2 && (c[len-2] == '\r' && c[len-1] == '\n')) {
		c[len-2] = c[len-1] = '\0';
	}else if (len >= 1 && (c[len-1] == '\r' || c[len-1] == '\n')) {
		c[len-1] = '\0';
	}
	return;
}

void errorReq (reqinfo* r, int num) {
	r -> error  = num;
	r -> status = 400;
	switch (num) {
	case 11:
		perror ("Header doesn't start from \"Host:\"\n");
		break;
	case 15:
		perror ("URI includes \'<\' or \"..\".");
		break;
	case 100:
		fprintf(stderr, "method is too long\n");
		break;
	default:
		fprintf(stderr, "Error %d\n", num);
	}
	return;
}



int invalidUri (char* b) {
	//check '<' and ".."
	int r = 0, i = 0;
	for (i = 0; i < (int) strlen(b); i++) {
		if (b[i] == '<') {
			r = 1;
		} else if (i+1 < (int) strlen(b) && b[i] == '.' && b[i+1] == '.') {
			r = 1;
		}
	}
	return r;
}


/*
	m = "GET /index.php HTTP/1.1"
 */

void parseMethod (reqinfo* r, char* b){
	char method[10];
	char uri[100];
	char ver[10];
	int u_head, v_head = 0;
	int tail=0;
	u_head = 0;
	while (u_head < (int) strlen(b) && b[u_head] != '\0') {
		if(b[u_head] == ' ') {
			b[u_head] = '\0';
			u_head++;
			break;
		}
		u_head++;
	}
	if (strlen(b) > 10) {
		fprintf (stderr, "%s", b);
		errorReq(r, 100);
		return;
	}
	strcpy(method, b);
	r->error = 0;

	//split by space
	v_head = 0;
	while (v_head < (int)strlen(b+u_head) && b[u_head+v_head] != '\0') {
		if (b[u_head+v_head] == ' ') {
			b[u_head+v_head] = '\0';
			v_head++;
			break;
		}
		v_head++;
	}
	if(100 < (int)strlen(b+u_head)) {
		perror("hogehoge");
		fprintf (stderr, "%s", b);
		errorReq(r, 100);
		return;
	}
	strcpy(uri, b+u_head);

	tail = 0;
	while(tail <(int) strlen(b+u_head+v_head) &&b[u_head+v_head+tail] != '\0') {
		if(b[u_head+v_head+tail] == '\r') {
			b[u_head+v_head+tail] = '\0';
			tail++;
			break;
		}
		tail++;
	}

	if(10 < (int) strlen(b+u_head+v_head)) {
		perror("fugafuga");
		fprintf(stderr, "%s\n", b+u_head+v_head);
		errorReq(r,100);
		return;
	}
	strcpy(ver, b+u_head+v_head);

	//set value in reqinfo
	//Method
	if (strcmp(method, "GET") == 0) {
		r -> method = 0;
	} else if (strcmp(method, "HEAD") == 0) {
		r -> method = 1;
	} else {
		errorReq(r, 3);
	}

	//URI
	if (0&&invalidUri (uri)) {
		errorReq(r, 15);
		//return b;
	} else {
		if (strlen(uri) > 1024) {
			r -> status = 400;
		} else {
			strcpy(r -> uri, uri);
		}
	}

	//version
	if (strcmp(ver, "HTTP/1.0") == 0) {
		r -> version = 0;
	} else if (strcmp(ver, "HTTP/1.1") == 0) {
		r -> version = 1;
	} else {
		fprintf(stderr, "VERSION ERROR:%s\n", ver);
		errorReq(r, 5);
	}

	return;
}




void parseHeader (reqinfo* r, char* top){
	char* head;
	char* body;
	int i=0, j=0;

	while (*top != '\0') {
		perror(top);
		//head
		i = 0;
		while (top[i] != ':') {
			if (top[i] == '\0') {
				perror("Invalid Request");
				fprintf(stderr, "%s\n", top);
				errorReq(r, 50);
				return;
			}
			i++;
		}
		top[i] = '\0';i++;
		while(top[i] == ' ') i++;

		//body
		j = i;
		while (top[j] != '\r' && top[j] != '\n') {
			fprintf(stderr, "%c", top[j]);
			if (top[j] == '\0') {
				perror("Invalid Request2");
				fprintf(stderr, "%s : %s\n", top, top+i);
				errorReq(r, 50);
				return;
			}
			j++;
		}
		top[j] = '\0'; j++;
		while(top[j] == ' ' || top[j] == '\r' || top[j] == '\n') j++;

		head = top;
		body = top + i;
		top  = top + j;
		perror("\n********************");
		fprintf(stderr,"head:%s\n",head);
		fprintf(stderr,"body:%s\n",body);
		perror("\n********************");
	}

	return;
}
