CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror

.PHONY: all test clean

all: csv_reader

csv_reader: csv.c main.c csv.h
	$(CC) $(CFLAGS) -o csv_reader.exe csv.c main.c

test: csv.c test_csv.c csv.h
	$(CC) $(CFLAGS) -o test_csv.exe csv.c test_csv.c
	./test_csv.exe

clean:
	rm -f *.exe *.o
