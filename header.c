#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "parser.h"

char* month[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
char* wday[7] = {"Sun\0", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};


char* getContentType (char* uri) {
	int len = strlen(uri);
	if (len > 4&&strcmp((uri+len-4), ".jpg") == 0) {
		return "image/jpeg";
	} else if (len > 4&&strcmp((uri+len-4), ".png") == 0) {
		return "image/png";
	} else {
		return "text/html";
	}
}
void getGMT (char* c) {

	time_t timer;
	struct tm *gmt;

	time(&timer);

	gmt = gmtime(&timer);

	sprintf(c, "%s, %02d %s %d %02d:%02d:%02d GMT",
					wday[gmt->tm_wday],
					gmt->tm_mday, month[gmt->tm_mon],
					gmt->tm_year+1900, gmt->tm_hour,
					gmt->tm_min, gmt->tm_sec
					);
	return;
}
