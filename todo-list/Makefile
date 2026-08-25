CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror

.PHONY: all test clean

all: todo

todo: todo.c main.c todo.h
	$(CC) $(CFLAGS) -o todo.exe todo.c main.c

test: todo.c test_todo.c todo.h
	$(CC) $(CFLAGS) -o test_todo.exe todo.c test_todo.c
	./test_todo.exe

clean:
	rm -f *.exe *.o
