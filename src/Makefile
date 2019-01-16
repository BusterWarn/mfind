CC = gcc
CFLAGS = -std=gnu11 -g -Wall -Wextra -Werror -Wmissing-declarations -Wmissing-prototypes -Werror-implicit-function-declaration -Wreturn-type -Wparentheses -Wunused -Wold-style-definition -Wundef -Wshadow -Wstrict-prototypes -Wswitch-default -Wunreachable-code

all:				mfind

mfind:				mfind.o queue.o parseMfind.o saferMemHandler.o
	$(CC) -pthread mfind.o queue.o parseMfind.o saferMemHandler.o -o mfind

mfind.o:			mfind.c mfind.h queue.h parseMfind.h
	$(CC) $(CFLAGS) -c mfind.c

queue.o: 			queue.c queue.h saferMemHandler.h
	$(CC) $(CFLAGS) -c queue.c

parseMfind.o:		parseMfind.c parseMfind.h saferMemHandler.h
	$(CC) $(CFLAGS) -c parseMfind.c

saferMemHandler.o:	saferMemHandler.c saferMemHandler.h
	$(CC) $(CFLAGS) -c saferMemHandler.c
	
clean:
	rm -f mfind *.o core
