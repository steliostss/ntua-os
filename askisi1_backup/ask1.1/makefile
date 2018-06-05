all: exec exec2

exec: main.o zing.o
	gcc -o exec main.o zing.o

exec2: main.o zing2.o
	gcc -o exec2 main.o zing2.o

main.o: main.c
	gcc -Wall -c main.c

zing2.o: zing2.c
	gcc -Wall -c zing2.c
