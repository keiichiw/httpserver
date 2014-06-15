#ifndef PARSER_H
#define PARSER_H 1
typedef struct {
	char* root;
	int status;
	int error;
	int method; // GET =0 HEAD=1
	char* uri;
	int version;//0 or 1
	char* host;
	char* user_agent;
	char* accept;
} reqinfo;
void chomp (char*);
char* parseMethod (reqinfo* , char*);
void parseHeader (reqinfo* , char* );
void all_free (reqinfo* );
void printReq(reqinfo*);
#endif
