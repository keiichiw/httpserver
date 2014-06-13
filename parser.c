#include <string.h>
#include <stdlib.h>
#include "parser.h"


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
reqinfo* errorReq (reqinfo* r, int num) {
	r -> error = num;
	return r;
}


/*
	m = "GET /index.php HTTP/1.1"
 */
reqinfo* parseMethod (reqinfo* r, char* m){
	char *u;
	char *v;
	int i;
	r->error = 0;
	chomp(m);

	while(m[i] != ' ') {
		i++;
		if (m[i] == '\0') return errorReq(r, 1);
	}
	u = m+i+1;
	while(*u != ' '&& *u != '\0') u++;
	m[i] = '\0';
	if (strcmp(m, "GET") == 0) {
		r -> method = 0;
	} else if (strcmp(m, "HEAD") == 0) {
		r -> method = 1;
	} else {
		return errorReq(r, 2);
	}

	i = 0;
	while(u[i] != ' ') {
		i++;
		if (u[i] == '\0') return errorReq(r, 3);
	}
	v = u+i+1;
	while(*v != ' '&& *v != '\0') v++;
	u[i] = '\0';
	r -> uri = (char*) malloc(strlen(u) * sizeof(char));
	strcpy(r -> uri, u);


	i = 0;
	while(v[i] != ' ') i++;
	v[i] = '\0';
	if (strcmp(v, "HTTP/1.0") == 0) {
		r -> version = 0;
	} else if (strcmp(v, "HTTP/1.1") == 0) {
		r -> version = 1;
	} else {
		return errorReq(r, 4);
	}


	return r;
}
