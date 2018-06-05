#
# Makefile
#
# Operating Systems, Exercise 4
#

CC = gcc
#CFLAGS = -Wall -g
CFLAGS = -Wall -O2 -g -ggdb -ggdb3 

all: scheduler scheduler-shell shell prog execve-example strace-test sigchld-example scheduler-priority

scheduler: scheduler.o proc-common.o functions.o
	$(CC) -o scheduler scheduler.o proc-common.o functions.o

scheduler-shell: scheduler-shell.o proc-common.o functions.o
	$(CC) -o scheduler-shell scheduler-shell.o proc-common.o functions.o

scheduler-priority: scheduler-priority.o proc-common.o functions.o
	$(CC) -o scheduler-priority scheduler-priority.o proc-common.o functions.o

scheduler-priority.o: scheduler-priority.c
	$(CC) -o scheduler-priority.o -c scheduler-priority.c

shell: shell.o proc-common.o functions.o
	$(CC) -o shell shell.o proc-common.o functions.o

functions.o: functions.c functions.h
	$(CC) -o functions.o -c functions.c

prog: prog.o proc-common.o
	$(CC) -o prog prog.o proc-common.o

execve-example: execve-example.o
	$(CC) -o execve-example execve-example.o

strace-test: strace-test.o
	$(CC) -o strace-test strace-test.o

sigchld-example: sigchld-example.o proc-common.o
	$(CC) -o sigchld-example sigchld-example.o proc-common.o

proc-common.o: proc-common.c proc-common.h
	$(CC) $(CFLAGS) -o proc-common.o -c proc-common.c

shell.o: shell.c proc-common.h request.h
	$(CC) $(CFLAGS) -o shell.o -c shell.c

scheduler.o: scheduler.c proc-common.h request.h
	$(CC) $(CFLAGS) -o scheduler.o -c scheduler.c

scheduler-shell.o: scheduler-shell.c proc-common.h request.h
	$(CC) $(CFLAGS) -o scheduler-shell.o -c scheduler-shell.c 

prog.o: prog.c
	$(CC) $(CFLAGS) -o prog.o -c prog.c

execve-example.o: execve-example.c
	$(CC) $(CFLAGS) -o execve-example.o -c execve-example.c

strace-test.o: strace-test.c
	$(CC) $(CFLAGS) -o strace-test.o -c strace-test.c

sigchld-example.o: sigchld-example.c
	$(CC) $(CFLAGS) -o sigchld-example.o -c sigchld-example.c 

clean:
	rm -f scheduler scheduler-priority scheduler-shell shell prog execve-example strace-test functions sigchld-example *.o
