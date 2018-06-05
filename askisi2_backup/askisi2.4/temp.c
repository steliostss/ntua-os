#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#include "tree.h"
#include "proc-common.h"

typedef struct tree_node* Tree;

#define SLEEP_TREE_SEC 5
#define SLEEP_PROC_SEC 10

void fork_tree(Tree curnode)
{
        unsigned int chiNo = curnode->nr_children;

        pid_t pid;
        unsigned i=0;

	for(i=0; i<chiNo; i++){
                pid = fork();
		if(pid < 0){
			perror("Error at children");
                        exit(9);
                }
    	        if(pid == 0){
			change_pname( (curnode->children+i)->name );
			printf("%s: Created\n", (curnode->children+i)->name);
                        fork_tree(curnode->children+i);
			printf("PID = %ld, name = %s is going to sleep\n", (long)getpid(), (curnode->children+i)->name);
			sleep(SLEEP_PROC_SEC);
			printf("PID = %ld, name = %s is awake\n", (long)getpid(), (curnode->children+i)->name);
			exit(0);
                }
		curnode->children[i].pid = pid;
	}

	/*
	 *	printf("I am %s and these are my children\n", curnode->name);
	 *	for (i = 0; i < chiNo; i++) {
	 *		printf("\tHi, I am %s\n", (curnode->children+i)->name);
	 *	}
	 */

	printf("PID = %ld, name = %s: I just woke up...\n", (long)getpid(), curnode->name);


	/* Open pipe */

	int pfd[2];
	if ( pipe(pfd) < 0 ){
		perror("pipe @ children");
	}

	/*
	 * Compute value and return to parent proc
	 * If chiNo == 0 return value, else must compute through
	 * info that comes from pipe
	 */

	int operands[2];
	int value;

	for (i=0; i < 2; i++) {
		if (read(pfd[0], &value, sizeof(value)) != sizeof(value)) {
			perror("Read from Pipe Failed");
			exit(1);
		}
		printf("PID: %ld (%s): Reading from pipe value no. %d : %d\n", (long)getpid(), curnode->name, i, value);
		operands[i] = value;

		// p = wait(&status); //Node B waiting
		//explain_wait_status(p, status);
	}

	int result;

	if (strcmp(curnode->name, "+") == 0)
		result = operands[0] + operands[1];
	else
		result = operands[0] * operands[1];

	if ( write(pfd[1], &result, sizeof(result) ) != sizeof(result) ) {
		perror("Pipe Write Failed");
		exit(1);
	}
	printf("PID: %ld: Wrote calculated result (%d) to Pipe %d\n", (long)getpid(), result, pfd[1]);
		sleep(SLEEP_PROC_SEC);
		exit(result);
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

int main(int argc, char *argv[])
{
	pid_t pid;
	int status;
	Tree root;

	int pfd[2];

	if (argc < 2){
		fprintf(stderr, "Usage: %s <tree_file>\n", argv[0]);
		exit(1);
	}

	/* Read tree into memory */
	root = get_tree_from_file(argv[1]);

	if( pipe(pfd) < 0 ){
		perror("pipe @ A");
	}

	/* Fork root of process tree */
	pid = fork();
	if (pid < 0) {
		perror("main: fork");
		exit(9);
	}

	if (pid == 0) {
		/* Child */
		change_pname(root->name);
		printf("%s: Created\n", root->name);
		fork_tree(root);
		exit(1);
	}

	/*
	 * Father
	 */
	/* for ask2-signals */
	// wait_for_ready_children(1);

	sleep(SLEEP_TREE_SEC);

	/* Print the process tree root at pid */
	show_pstree(pid);

	/* for ask2-signals */
	// kill(pid, SIGCONT);

        int final_val = 0;

        if ( read(pfd[0], &final_val, sizeof(final_val) ) != sizeof(final_val) ){
                perror("error @ read");
                exit(10);
        }

	/* Wait for the root of the process tree to terminate */
	wait(&status);
	explain_wait_status(pid, status);

	printf("Final value is: %d\n", final_val);

	return 0;
}
