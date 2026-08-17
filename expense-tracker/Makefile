CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror

.PHONY: all test clean

all: expense_tracker

expense_tracker: expense.c main.c expense.h
	$(CC) $(CFLAGS) -o expense_tracker.exe expense.c main.c

test: expense.c test_expense.c expense.h
	$(CC) $(CFLAGS) -o test_expense.exe expense.c test_expense.c
	./test_expense.exe

clean:
	rm -f *.exe *.o
