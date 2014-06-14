all:
	gcc -Wall parser.c sock.c server.c httpserver.c -o httpserver

clean:
	rm -f httpserver *~
