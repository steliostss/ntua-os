.PHONY: all clean

all: exec

CC = gcc
CFLAGS = -g -Wall -O2
SHELL= /bin/bash

exec: main.o tree.o proc-common.o
	$(CC) $(CFLAGS) $^ -o $@

%.s: %.c
	$(CC) $(CFLAGS) -S -fverbose-asm $<

%.o: %.c
	$(CC) $(CFLAGS) -c $<

%.i: %.c
	gcc -Wall -E $< | indent -kr > $@

clean:
	rm -f *.o exec tree main proc-common

