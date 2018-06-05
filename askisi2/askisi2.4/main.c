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

#define SLEEP_TREE_SEC 3
#define SLEEP_PROC_SEC 10

void fork_tree(Tree curnode, int pfd)
{
        unsigned int chiNo = curnode->nr_children;

	char* name = curnode->name;

	change_pname(name);
	printf("Wrote value %s in proc\n", name);

	//if its a leaf
	if(chiNo == 0) {
		int value = atoi(name);
		if ( write(pfd, &value, sizeof(value) ) != sizeof(value) ) {
			perror("Pipe write Failed\n");
			exit(1);
		}
		printf("Leaf: %s, just closed write side of pipe\n", name);
		printf("PID: %ld, %s: Wrote value: %d on Pipe %d and exiting... \n", (long)getpid(), name, value, pfd);
//		sleep(SLEEP_PROC_SEC);
		exit(getpid());
	}

	//if its NOT a leaf
	pid_t p;
	int i=0;
	int fd[2][2];
	if( pipe(fd[0]) < 0 || pipe(fd[1]) < 0 ) {
		perror("error @ pipe");
		exit(15);
	}
	printf("Pipe created\n");
	for(i=0; i<2; i++) {
		p = fork();
		if ( p < 0 ) {
			perror("fork failed");
			exit(15);
		}
		if ( p == 0 ) {
			close(fd[i][0]);
			fork_tree(curnode->children+i, fd[i][1]);
			printf("Child created\n");
		}
	}

	int operands[2];
	int value;
	for (i=0; i < 2; i++) {
	        if (read(fd[i][0], &value, sizeof(value)) != sizeof(value)) {
	                perror("Read from Pipe Failed");
	                exit(1);
	        }
		close(fd[i][1]);
	        printf("I'm: (%s): Reading from pipe %d no: %d\n", curnode->name, i, value);
	        operands[i] = value;
	        // p = wait(&status); //Node B waiting
	        //explain_wait_status(p, status);
		printf("Child: (%s) just closed read size of node\n", curnode->children[i].name);
	}
	int result;

	if (strcmp(curnode->name, "+") == 0)
	        result = operands[0] + operands[1];
	else
	        result = operands[0] * operands[1];
	if ( write(pfd, &result, sizeof(result) ) != sizeof(result) ) {
	        perror("Pipe Write Failed");
	        exit(1);
	}
	close(fd[0][1]);
	close(fd[1][1]);
	printf("Node: (%s) just closed write side of pipe\n", curnode->name);
	sleep(SLEEP_PROC_SEC);
	exit(result);
}

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
		close(pfd[0]);
                fork_tree(root, pfd[1]);
                exit(1);
        }

        /*
         * Father
         */

	/* for ask2-signals */

        sleep(SLEEP_TREE_SEC);

        /* Print the process tree root at pid */
        show_pstree(pid);

        /* for ask2-signals */

        int final_val = 0;

        if ( read(pfd[0], &final_val, sizeof(final_val) ) != sizeof(final_val) ){
                perror("error @ read");
                exit(10);
        }

        /* Wait for the root of the process tree to terminate */

	close(pfd[1]);
	printf("Allfather just closed read side\n");

	wait(&status);
        explain_wait_status(pid, status);

        printf("Final value is: %d\n", final_val);

        return 0;
}
