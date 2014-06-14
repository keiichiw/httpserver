#ifndef SERVER_H
#define SERVER_H 1
#include "parser.h"
reqinfo* getRequest (int cSock, char* droot);
void sendResponse (reqinfo* r, int cSock);
#endif
