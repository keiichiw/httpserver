all:
	gcc -Wall server.c httpserver.c -o httpserver

clean:
	rm -f httpserver *~
