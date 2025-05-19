// Makefile for building the server and client

CC = gcc
CFLAGS = -Wall -Wextra

all: bank_server bank_client

bank_server: bank_server.c bank_common.h
	$(CC) $(CFLAGS) -o bank_server bank_server.c

bank_client: bank_client.c bank_common.h
	$(CC) $(CFLAGS) -o bank_client bank_client.c

clean:
	rm -f bank_server bank_client

.PHONY: all clean