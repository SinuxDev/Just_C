CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror

.PHONY: all test clean

all: grep

grep: grep.c main.c grep.h
	$(CC) $(CFLAGS) -o grep.exe grep.c main.c

test: grep.c test_grep.c grep.h
	$(CC) $(CFLAGS) -o test_grep.exe grep.c test_grep.c
	./test_grep.exe

clean:
	rm -f *.exe *.o
