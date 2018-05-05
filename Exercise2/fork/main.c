#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "proc-common.h"

#define SLEEP_PROC_SEC  2
#define SLEEP_TREE_SEC  3

/*
 * Create this process tree:
		A
	   / \
	  C   B
	  	   \
	  	    D
 */

void fork_procs(pid_t a)
{
    int status;
    change_pname("A");
    printf("A: Sleeping...\n");

    pid_t b = fork();
    if (b < 0){
        perror("Error @ B");
        exit(1);
    }
    if(b == 0){
        change_pname("B");
        printf("B: Sleeping...\n");
        pid_t d = fork();
        if (d < 0){
            perror("Error @ D");
            exit(1);
        }
        if (d == 0){
            change_pname("D");
            printf("D: Sleeping...\n");
            sleep(SLEEP_PROC_SEC);

            printf("D: Exiting...\n");
            exit(13);
        }

        d = wait(&status);
        explain_wait_status(b, status);
        printf("Kill D\n");
//        sleep(SLEEP_PROC_SEC);
        printf("B: Exiting...\n");
        exit(19);
    }

    pid_t c = fork();
    if (c < 0){
        perror("Error @ C");
        exit(1);
    }
    if (c==0){
        change_pname("C");
        printf("C: Sleeping...\n");
        sleep(SLEEP_PROC_SEC);
        printf("C: Exiting...\n");
        exit(17);
    }

    //C
    pid_t pid1 = wait(&status);
    explain_wait_status(0, status);
    printf("Kill C\n");

    //B
    pid_t pid2 = wait(&status);
    explain_wait_status(c, status);
    printf("Kill B\n");

    printf("A: Exiting...\n");
//    sleep(SLEEP_PROC_SEC);
    exit(16);
}

/*
 * The initial process forks the root of the process tree,
 * waits for the process tree to be completely created,
 * then takes a photo of it using show_pstree().
 *
 * How to wait for the process tree to be ready?
 * In ask2-{fork, tree}:
 *      wait for a few seconds, hope for the best.
 * In ask2-signals:
 *      use wait_for_ready_children() to wait until
 *      the first process raises SIGSTOP.
  */

int main(void)
{
    pid_t pid;
    int status;

    pid = fork(); //A
    if (pid < 0) {
        perror("main: fork");
        exit(1);
    }
    if (pid == 0) {
        fork_procs(pid);
        exit(1);
    }

    pid = wait(&status);
    explain_wait_status(pid, status);

    return 0;
}
