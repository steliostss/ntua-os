#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "proc-common.h"
#include "tree.h"

#define SLEEP_PROC_SEC  10
#define SLEEP_TREE_SEC  5

typedef struct tree_node* Tree;

void fork_tree(Tree curnode)
{
        unsigned int chiNo = curnode->nr_children;

	if(chiNo != 0){
		pid_t pid;
		unsigned i=0;
		int status;
		for(i=0; i<chiNo; i++){
			pid = fork();
			if(pid < 0){
				perror("Error at children");
				exit(9);
			}
			if(pid == 0){
				change_pname((curnode->children+i)->name);
				printf("%s: Created\n", (curnode->children+i)->name);
				fork_tree(curnode->children+i);
			}
		}
		for(i=0; i<chiNo; i++){
			pid = wait(&status);
			explain_wait_status(pid, status);
		}
		printf("%s: Exiting...\n", curnode->name);
		exit(getpid());
	}
	else {
		printf("%s: Sleeping...\n", curnode->name);
		sleep(SLEEP_PROC_SEC);
		printf("%s: Exiting...\n", curnode->name);
		exit(getpid());
	}
}


int main (int argc, char **argv)
{

	pid_t root_pid;
	Tree root = get_tree_from_file(argv[1]);
	int status;

	root_pid  = fork();
	if (root_pid < 0){
        	perror("Error @ root\n");
        	exit(1);
    	}
    	if (root_pid==0){
        	change_pname(root->name);
        	printf("%s: Created\n", root->name);
		fork_tree(root);
		exit(getpid());
	}

	sleep(SLEEP_TREE_SEC);

        show_pstree(root_pid);

	root_pid = wait(&status);
	explain_wait_status(root_pid, status);

	return 0;
}
