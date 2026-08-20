CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror

.PHONY: all test clean

all: contact_book

contact_book: contact.c main.c contact.h
	$(CC) $(CFLAGS) -o contact_book.exe contact.c main.c

test: contact.c test_contact.c contact.h
	$(CC) $(CFLAGS) -o test_contact.exe contact.c test_contact.c
	./test_contact.exe

clean:
	rm -f *.exe *.o
