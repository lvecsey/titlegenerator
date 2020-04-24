
CC=gcc

CFLAGS=-O3 -Wall -g -pg

LIBS=-lfcgi

titlegenerator : titlegenerator.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) $(LIBS)

