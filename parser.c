#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#define BUFF_SIZE 20480

void all_free (reqinfo* r) {
	free(r -> host);
	free(r -> user_agent);
	free(r -> accept);
	free(r -> uri);
	free(r);
	return;
}
void printReq (reqinfo* r) { //for debug
	fprintf(stderr, "PrintReq----------\n");
	fprintf(stderr, "Root:%s\n", r -> root);
	fprintf(stderr, "Debug num:%d\n", r -> error);
	fprintf(stderr, "Method:%s\n", (r -> method == 0)?"GET":"HEAD");
	fprintf(stderr, "Status: %d\n", r -> status);
	fprintf(stderr, "URI:%s\n", r -> uri);
	fprintf(stderr, "HTTP/1.%d\n", r -> version);
	fprintf(stderr, "Host:%s\n", r -> host);
	fprintf(stderr, "User-Agent:%s\n", r -> user_agent);
	fprintf(stderr, "Accept:%s\n", r -> accept);
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
	for (i = 0; i < strlen(b); i++) {
		if (b[i] == '<') {
			r = 1;
		} else if (i+1 < strlen(b) && b[i] == '.' && b[i+1] == '.') {
			r = 1;
		}
	}
	return r;
}


/*
	m = "GET /index.php HTTP/1.1"
 */

char* parseMethod (reqinfo* r, char* b){
	char *method;
	char *uri;
	char *ver;
	char *head;
	int i = 0;
	if (strlen(b) > BUFF_SIZE) {
		fprintf (stderr, "%s", b);
		errorReq(r, 100);
		return b;
	}
	method = (char*) malloc(strlen(b) * sizeof(char));
	strcpy(method, b);
	r->error = 0;

	//split by space
	while(method[i] != ' ') {
		if (method[i] == '\0') {errorReq(r, 1);return b;}
		i++;
	}
	method[i] = '\0';
	uri = method+i+1;
	i = 0;
	while(uri[i] != ' ') {
		if (uri[i] == '\0') {errorReq(r, 2);return b;}
		i++;
	}
	uri[i] = '\0';
	ver = uri+i+1;
	i = 0;
	while(ver[i] != '\r' && ver[i] != '\n' &&ver[i] != '\n') {
		if (ver[i] == '\0') {errorReq(r, 3);return b;}
		i++;
	}
	ver[i] = '\0';
	head = ver+i+1;
	i = 0;
	while(head[i] == '\r'||head[i] == '\n') i++;
	head = head + i;

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
		r -> uri = (char*) malloc(strlen(uri) * sizeof(char));
		if (r -> uri == NULL) {

		}
		strcpy(r -> uri, uri);
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

	free(method);
	return head;
}

void evalHeader(reqinfo* r, char* head, char* body) {
	if (strcmp(head, "Host") == 0) {
		r -> host = (char*) malloc(strlen(body) * sizeof(char));
		strcpy(r -> host, body);
	} else if (strcmp(head, "User-Agent") == 0) {
		r -> user_agent = (char*) malloc(strlen(body) * sizeof(char));
		strcpy(r -> user_agent, body);
	} else if (strcmp(head, "Accept") == 0) {
		r -> accept = (char*) malloc(strlen(body) * sizeof(char));
		strcpy(r -> accept, body);
	} else {
		//fprintf(stderr, "Header not found-> %s%s\n", head, body);
	}
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
		perror("********1***********::");
		fprintf(stderr,"head:%s\n",head);
		fprintf(stderr,"body:%s\n",body);
		perror("********2***********::");
		evalHeader (r, head, body);
	}

	return;
}
