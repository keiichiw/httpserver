#ifndef SERVER_H
#define SERVER_H 1
#include "parser.h"
void getRequest (reqinfo*, int);
void sendResponse (reqinfo* r, int cSock);
#endif
