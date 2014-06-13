#ifndef PARSER_H
#define PARSER_H 1
typedef struct {
	int version;//0 or 1
	int method; // GET =0 HEAD=1
	char* host;
	char* uri;
	int error;
} reqinfo;
void chomp (char*);
void parseMethod (reqinfo* , char*);
void parseHost (reqinfo* , char* );
#endif
