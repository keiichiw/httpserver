CC		= gcc
CFLAGS		= -Wall
TARGET	= httpserver
OBJS	= parser.o sock.o header.o server.o httpserver.o
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

clean:
	rm -f $(TARGET) *~ *.o

.c.o:
	$(CC) $(CFLAGS) -c $<
