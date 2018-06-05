.PHONY: all clean

all: main

CC = gcc
CFLAGS = -g -Wall -O2
SHELL= /bin/bash

main: main.o proc-common.o
	$(CC) $(CFLAGS) $^ -o $@
clean: 
	rm -f *.o main
