#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "parser.h"


void all_free (reqinfo* r) {
	free(r -> host);
	free(r -> user_agent);
	free(r -> accept);
	free(r -> uri);
	free(r);
	return;
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
	char *m;
	char *u;
	char *v;
	char *head;
	int i = 0;
	if (strlen(b) > 256) {
		errorReq(r, 100);
		return b;
	}
	m = (char*) malloc(256 * sizeof(char));
	strcpy(m, b);
	r->error = 0;
	chomp(m);

	while(m[i] != ' ') {
		if (m[i] == '\0') {
			errorReq(r, 1);
			return b;
		}
		i++;
	}
	u = m+i+1;
	while(*u == ' '&& *u != 0) u++;
	m[i] = '\0';
	if (strcmp(m, "GET") == 0) {
		r -> method = 0;
	} else if (strcmp(m, "HEAD") == 0) {
		r -> method = 1;
	} else {
		errorReq(r, 2);
		return b;
	}

	i = 0;
	while(u[i] != ' ') {
		if (u[i] == '\0') {
			errorReq(r, 3);
			return b;
		}
		i++;
	}

	v = u+i+1;
	while(*v == ' '&& *v != '\0') v++;
	u[i] = '\0';
	if (invalidUri (u)) {
		errorReq(r, 15);
		return b;
	}
	r -> uri = (char*) malloc(strlen(u) * sizeof(char));
	strcpy(r -> uri, u);


	i = 0;
	while (v[i] != ' ' && v[i] != 0) i++;
	head = v + i;
	if (strcmp(v, "HTTP/1.0") == 0) {
		r -> version = 0;
	} else if (strcmp(v, "HTTP/1.1") == 0) {
		r -> version = 1;
	} else {
		fprintf(stderr, "v = %s\n", v);
		errorReq(r, 4);
		return b;
	}

	i = 0;
	while (*head == ' '||*head == '\r' || *head == '\n') head++;
	if (*head == '\0') {
		perror("Wrong Request");
		errorReq(r, 5);
		return b;
	}
	return head;
}

void evalHeader(reqinfo* r, char* head, char* body) {
	if (strcmp(head, "Host:") == 0) {
		r -> host = (char*) malloc(strlen(body) * sizeof(char));
		strcpy(r -> host, body);
	} else if (strcmp(head, "User-Agent:") == 0) {
		r -> user_agent = (char*) malloc(strlen(body) * sizeof(char));
		strcpy(r -> user_agent, body);
	} else if (strcmp(head, "Accept:") == 0) {
		r -> accept = (char*) malloc(strlen(body) * sizeof(char));
		strcpy(r -> accept, body);
	} else {
		fprintf(stderr, "Header not found-> %s%s\n", head, body);
	}
}

void parseHeader (reqinfo* r, char* top){
	char* head;
	char* body;
	int i=0, j=0;

	while (*top != '\0') {

		//head
		i = 0;
		while (top[i] != ' ') {
			if (top[i] == '\0') {
				perror("Invalid Request");
				errorReq(r, 50);
				return;
			}
			i++;
		}
		top[i] = '\0';i++;
		while(top[i] == ' '|| top[i] == '\r' || top[i] == '\n') i++;

		//body
		j = i;
		while (top[j] != ' ' && top[j] != '\r' && top[j] != '\n') {
			if (top[j] == '\0') {
				perror("Invalid Request");
				errorReq(r, 50);
				return;
			}
			j++;
		}
		top[j] = '\0'; j++;
		while(top[j] == ' '|| top[j] == '\r' || top[j] == '\n') j++;

		head = top;
		body = top + i;
		top  = top + j;
		evalHeader (r, head, body);
	}

	return;
}
