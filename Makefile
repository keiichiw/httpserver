all:
	gcc -Wall parser.c server.c httpserver.c -o httpserver

clean:
	rm -f httpserver *~
