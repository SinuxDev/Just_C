CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror

.PHONY: all test clean

all: word-freq

word-freq: freq.c main.c freq.h
	$(CC) $(CFLAGS) -o word-freq.exe freq.c main.c

test: freq.c test_freq.c freq.h
	$(CC) $(CFLAGS) -o test_freq.exe freq.c test_freq.c
	./test_freq.exe

clean:
	rm -f *.exe *.o
