CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror

.PHONY: all test clean

all: tic_tac_toe

tic_tac_toe: ttt.c main.c ttt.h
	$(CC) $(CFLAGS) -o tic_tac_toe.exe ttt.c main.c

test: ttt.c test_ttt.c ttt.h
	$(CC) $(CFLAGS) -o test_ttt.exe ttt.c test_ttt.c
	./test_ttt.exe

clean:
	rm -f *.exe *.o
