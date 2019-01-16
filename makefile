#makefile for count.c

CC=gcc
CGLAGS = -g -Wall

all: count

hello: count.c
	$(CC) $(CFLAGS) -o count count.c

clean:
	rm count