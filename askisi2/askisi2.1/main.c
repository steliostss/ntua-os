#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "proc-common.h"

#define SLEEP_PROC_SEC  10
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

    pid_t b, c, d;

    int status;

    change_pname("A");
    printf("A: Created\n");
    printf("A: Sleeping...\n\n");

    b = fork();
    if (b < 0){
        perror("Error @ B\n");
        exit(1);
    }
    if(b == 0){
        change_pname("B");
        printf("B: Created\n");
        printf("B: Sleeping...\n");
        d = fork();
        if (d < 0){
            perror("Error @ D\n");
            exit(1);
        }
        if (d == 0){
            change_pname("D");
	    printf("D: Created\n");
            printf("D: Sleeping...\n");
            sleep(SLEEP_PROC_SEC);

            printf("D: Exiting...\n");
            exit(13);
        }

        d = wait(&status);
        explain_wait_status(d, status);
        sleep(SLEEP_PROC_SEC);
        printf("B: Exiting...\n");
        exit(19);
    }

    c = fork();
    if (c < 0){
        perror("Error @ C\n");
        exit(1);
    }
    if (c==0){
        change_pname("C");
	printf("C: Created\n");
        printf("C: Sleeping...\n");
        sleep(SLEEP_PROC_SEC);
        printf("C: Exiting...\n");
        exit(17);
    }

    //C
    c = wait(&status);
    explain_wait_status(c, status);

    //B
    b = wait(&status);
    explain_wait_status(b, status);

    printf("A: Exiting...\n");
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

    sleep(SLEEP_TREE_SEC);
    show_pstree(getpid());
    pid = wait(&status);
    explain_wait_status(pid, status);

    return 0;
}
