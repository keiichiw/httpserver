#ifndef PARSER_H
#define PARSER_H 1
typedef struct {
	char root[100];
	int status;
	int error;
	int method; // GET =0 HEAD=1
	char uri[100];
	int version;//0 or 1
	/*
	char* host;
	char* user_agent;
	char* accept;
	*/
} reqinfo;
void chomp (char*);
void parseMethod (reqinfo* , char*);
void all_free (reqinfo* );
void printReq(reqinfo*);
#endif
