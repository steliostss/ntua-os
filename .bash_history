cd oslab/
cd code/
ls -l
cd sched/
ls -l
cd ..
cp sched ~/askisi4
cp sched ~/askisi4 -rf
cd ..
cd oslabd15/askisi4
ls -l
sched
cd sched
ls -l
Make
make
ls -l
make clean
ls -l
make
ls -l
ls -g
./execve-example 
reset
ls -l
./scheduler
./scheduler prog
./scheduler add
./scheduler-shell 
./
./prog
./scheduler prog
./scheduler prog prog prog prog
reset
make clean
cd ..
cd ../askisi3
ls -g
make clean
make
./mandel 15
./mandel 1
./mandel 80
student-pv
student-pc
ls -g
cd askisi2
ls -g
cd askisi2.4
make
make clean
make
ls -g
./exec expr.tree
cd ~
man sigchld
man signals
man sig
man nano
MAN MUTEX
man gcc
man 7 gcc
man 7 undocumented
man 2 sigkill
man sigkill
man 7 signal
reset
exit
cd ..
ls -g
ls
cd oslab-assist/
exit
student-pc
exit
reset
clear
exit
clear
reset
clear
reset
ls
ls -g
cd askisi4
ls -g
make clean
make
make clean
cd
ls
cd askisi4
ls -g
nano VAGGELI_TI_LEEI_REEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE 
rm VAGGELI_TI_LEEI_REEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE 
ls -g
reset
ls -g
nano scheduler.c
nano scheduler.creset
exit
student-pc
exi
exit
cd askisi4
ls -g
nano scheduler.c
reset
nano scheduler.c
exit
student-pc
exit
reset
nano askisi4/scheduler.c
exit
student-pc
exit
student-pc
exitexit
cd askisi4
nano functions.h
make
nano functions.h
exit
student-pc
exit
cd askisi4
nano functions.c
make
make clean
nano function.c
nano functions.c
nano scheduler.c
exit
student-pc
exit
student-pc
cd askisi4
ls -g
nano functions.h
student-pc
cd askisi4
ls -g
nano scheduler.c
nano functions.h
nano functions.c
nano scheduler.c
nano functions.h
nano functions.c
student-pc
cd askisi4
nano functions.c
student-pc
cd askisi4
ls -g
nano scheduler.c
nano functions.h
cd askisi4
reset
nano functions.h
nano scheduler.c
make
nano scheduler.c
make
reset
nano functions.h
make
./scheduler prog prog
nano prog.c
make
nano prog.c
make
make clean
reset
exit
student-pc
exit
reset
cd askisi4
nano scheduler.c
nano functions.c
nano Makefile 
make
nano Makefile 
make
make clean
reset
make
nano scheduler.v
nano scheduler.c
make
reset
make
ls -g
make clean
reset
nano functions.c
make
reset
nano scheduler.c
make
./scheduler prog prog prog prog
nano scheduler.c
nano functions.c
make
./scheduler prog prog
nano scheduler.c
make
./scheduler prog prog
nano scheduler.c
make
./scheduler prog prog
nano scheduler.c
make
./scheduler prog prog
nano scheduler.c
make
./scheduler prog prog
nano scheduler.c
make
./scheduler prog prog
make
nano scheduler.c
make
./scheduler prog prog
nano scheduler.c
make
./scheduler prog prog
nano scheduler.c
make
./scheduler prog prog
nano scheduler.c
make
./scheduler prog prog
exit
student-pc
exit
student-pc
cd askisi
cd askisi4
reset
make
nano functions.h
reset
make
reset
nano functions.c
make
nano Makefile
nano Makefile 
y
make
make clean
reset
make
ls -g
reset
cd askisi4
nano functions.c
exi
exit
student-pc
exit
cd askisi4
nano functions.h
exit
student-pc
exit
cd askisi4
ls -g
rm scheduler.c.save
make
ls -g
./shell
./shell prog
./shell 1
./shell 2 4
reset
ls -g
make clena
make clean
clean
clear
ls -g
nano scheduler-shell.c
nano execve-example.c
make execve
make
nano scheduler-shell.c
make
nano scheduler-shell.c
make
clear
ls g-
ls -g
./execve-example 
nano prog.c
make
nano prog.c
./execve-example 
nano prog.c
make
./execve-example 
nano execve-example.c
nano scheduler-shell.c
rm scheduler-shell.c
nano scheduler-shell.c
make
rm functions.*
make clean
clear
ls -g
nano functions.h
nano functions.c
ls -g
nano scheduler-priority.c
nano Makefile
rm Makefile
nano Makefile
#
# Makefile
#
# Operating Systems, Exercise 4
#
CC = gcc
#CFLAGS = -Wall -g
CFLAGS = -Wall -O2 -g -ggdb -ggdb3
all: scheduler scheduler-shell shell prog execve-example strace-test sigchld-example scheduler-priority
scheduler: scheduler.o proc-common.o helper.o
$(CC) -o scheduler scheduler.o proc-common.o helper.o
scheduler-shell: scheduler-shell.o proc-common.o helper.o
$(CC) -o scheduler-shell scheduler-shell.o proc-common.o helper.o
scheduler-priority: scheduler-priority.o proc-common.o helper.o
$(CC) -o scheduler-priority scheduler-priority.o proc-common.o helper.o
scheduler-priority.o: scheduler-priority.c
$(CC) -o scheduler-priority.o -c scheduler-priority.c
shell: shell.o proc-common.o helper.o
$(CC) -o shell shell.o proc-common.o helper.o
helper.o: helper.c helper.h
$(CC) -o helper.o -c helper.c
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
rm -f scheduler scheduler-shell shell prog execve-example strace-test sigchld-example *.o
reset
nano Makefile
make
nano Makefile
ls -g
./scheduler prog prog
reset
make
reset
make
nano Makefile
ls -g
make clean
ls -g
nano Makefile
make
nano Makefile
make clean
reset
make
ls -g
make clean
exit
student-pc
exit
nano askisi4/scheduler.c
exit
student-pc
exit
student-pc
exit
cd askisi4
ls -g
make clean
make
./scheduler-shell
./scheduler-shell prog prog
make clean
ls -g
nano Makefile
make clean
ls -g
rm scheduler-priority.c
nano scheduler-priority.c
make'
make
ls -g
make clean
ls -g
rm scheduler-priotiry.c
rm scheduler-priority.c
clear
nano scheduler-priority.c
make
ls -g
./scheduler-shell prog prog
./scheduler-shell prog
exirt
exit
student-pc
reset
make clean
ls -g
cd askisi4
make clean
ls -g
nano scheduler-shell.c
make
./scheduler-shell prog
nano scheduler-shell.c
reset
make
./scheduler-shell prog
nano scheduler-shell.c
make
./scheduler-shell prog
nano scheduler-shell.c
make
./scheduler-shell prog
man sigalrm
man signals
man signal
nano scheduler.c
make
make clean
make
ls -g
make clean
make
./scheduler-shell prog prog
nano scheduler-shell.c
nano functions.c
make
./scheduler-shell prog prog
nano functions.c
make
nano functions.c
nano scheduler.c
make
./scheduler-shell prog prog
exit
student-pc
reset
cd askisi4
make clean
ls -g
rm functions.c.save 
rm Makefile.save 
ls -g
nano prog.c
make
nano prog.c
ls -g
./shell
./shell prog prog
./scheduler prog prog
./scheduler-shell prog prog
k 1
nano scheduler.c
student-pc
cd askisi4
make clean
ls -g
make
./scheduler prog prog
nano scheduler.c
nano scheduler-shell.c
cd askisi4
ls
./shell prog prog prog
./scheduler-shell prog prog prog
ls
./scheduler-priority prog prog prog
exit
