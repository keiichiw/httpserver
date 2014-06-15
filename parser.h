#ifndef PARSER_H
#define PARSER_H 1
typedef struct {
	int version;//0 or 1
	int method; // GET =0 HEAD=1
	int status;
	char* host;
	char* user_agent;
	char* accept;
	char* uri;
	int error;
} reqinfo;
void chomp (char*);
char* parseMethod (reqinfo* , char*);
void parseHeader (reqinfo* , char* );
void all_free (reqinfo* );
#endif
