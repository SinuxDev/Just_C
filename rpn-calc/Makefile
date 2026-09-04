CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror

.PHONY: all test clean

all: rpn

rpn: rpn.c main.c rpn.h
	$(CC) $(CFLAGS) -o rpn.exe rpn.c main.c

test: rpn.c test_rpn.c rpn.h
	$(CC) $(CFLAGS) -o test_rpn.exe rpn.c test_rpn.c
	./test_rpn.exe

clean:
	rm -f *.exe *.o
