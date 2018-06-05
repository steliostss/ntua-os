#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <assert.h>

#include <sys/wait.h>
#include <sys/types.h>

#include "proc-common.h"
#include "request.h"

//Created this
#include "functions.h"

/* Compile-time parameters. */
#define SCHED_TQ_SEC 2                /* time quantum */
#define TASK_NAME_SZ 10               /* maximum size for a task's name */

process_list* p_list;

/*
 * SIGALRM handler
 */
static void sigalrm_handler(int signum)
{
	printf("\n*** SCHEDULER: Going to stop process [id]: %d ***\n", p_list->head->id);
	kill(p_list->head->pid, SIGSTOP);
}

/*
 * SIGCHLD handler
 */
static void
sigchld_handler(int signum)
{
	int status;
	pid_t pid;

	for(;;)
	{
		/*
		 * Check if any zombie-child exist. If any exists reap them and return its exit status
		 * If any child has stopped but hasn't report yet make them report to the questioning process
		 */

		pid = waitpid(-1, &status, WUNTRACED | WNOHANG);

		//Check if head process changed status
		if (pid < 0) {
			perror("waitpid < 0");
			exit(1);
		} else if (pid == 0) {
			break;
		} else if (pid > 0) {
			if (pid == p_list->head->pid) {
				process *p;

				//Process has stopped
				if(WIFSTOPPED(status)) {
					printf("*** SCHEDULER: STOPPED: Process [name]: %s  [id]: %d ***\n",
						p_list->head->name, p_list->head->id);
					p = get_next(p_list);
				} else if (WIFEXITED(status)) {
					printf("*** SCHEDULER: EXITED: Process [name]: %s  [id]: %d ***\n",
						p_list->head->name, p_list->head->id);
					p = pop(p_list);
					free_process(p);
					if(empty (p_list)) {
						printf("*** SCHEDULER: Empty list, nothing to schedule. ***\n");
						exit(0);
					}
					p = p_list->head;
				} else if (WIFSIGNALED(status)) {
					printf("*** SCHEDULER: Child killed by signal: Process [name]: %s [id]: %d ***\n",
						p_list->head->name, p_list->head->id);
					p = pop(p_list);
					free_process(p);
					if (empty(p_list)) {
						printf("*** SCHEDULER: Empty list, nothing to schedule. ***\n");
						exit(0);
					}
					p = p_list->head;
				} else {
					printf("*** SCHEDULER: Something strange happened with:	Process [name]: %s  [id]: %d ***\n",
					p_list->head->name, p_list->head->id);
					exit(1);
				}

				printf("*** SCHEDULER: Next process selected. ***\n*** Process [name]: %s, [id]: %d ***\n\n",
					p->name, p->id);
				//next process selected
				kill(p->pid, SIGCONT);
				alarm(SCHED_TQ_SEC);
			} else {
				/*
				 * Handle the case that a different than the
				 * head process has changed status
				 */

				process *pr = erase_proc_by_pid(p_list, pid);
				if( pr != NULL )
				{
					printf("*** SCHEDULER: A process other than the head has changed status unexpectedly: Process [name]: %s [id]: %d ***\n",
					pr->name, pr->id);
					free_process(pr);
				}
			}
		}
	}
}
/* Install two signal handlers.
 * One for SIGCHLD, one for SIGALRM.
 * Make sure both signals are masked when one of them is running.
 */
static void
install_signal_handlers(void)
{
	sigset_t sigset;
	struct sigaction sa;

	sa.sa_flags = SA_RESTART;

	sigemptyset(&sigset);
	sigaddset(&sigset, SIGCHLD);
	sigaddset(&sigset, SIGALRM);
	sa.sa_mask = sigset;

	sa.sa_handler = sigchld_handler;
	if (sigaction(SIGCHLD, &sa, NULL) < 0) {
		perror("sigaction: sigchld");
		exit(1);
	}

	sa.sa_handler = sigalrm_handler;
	if (sigaction(SIGALRM, &sa, NULL) < 0) {
		perror("sigaction: sigalrm");
		exit(1);
	}

	/*
	 * Ignore SIGPIPE, so that write()s to pipes
	 * with no reader do not result in us being killed,
	 * and write() returns EPIPE instead.
	 */
	if (signal(SIGPIPE, SIG_IGN) < 0) {
		perror("signal: sigpipe");
		exit(1);
	}
}

int main(int argc, char *argv[])
{
	int nproc;
	/*
	 * For each of argv[1] to argv[argc - 1],
	 * create a new child process, add it to the process list.
	 */
	nproc = argc - 1;
	if (nproc == 0) {
		fprintf(stderr, "*** SCHEDULER: No tasks. Exiting... ***\n");
		exit(1);
	}

	p_list = initialize_empty_list();

	int i;
	for (i = 1; i < argc; i++)
	{
		pid_t pid;
		pid = fork();
		if (pid < 0) {
			perror("fork");
			exit(1);
		}
		if (pid == 0)
		{
			printf("test");
			raise(SIGSTOP);
			char filepath[TASK_NAME_SZ];
			sprintf(filepath, "./%s", argv[i]);
			//TODO
			char* args[] = {filepath, NULL};
			if ( execvp(filepath, args) )
			{
				perror("execvp");
				exit(1);
			}
		}
		process *p = process_create(pid, argv[i]);
		push(p_list, p);
		printf("Process name: %s id: %d is created.\n", argv[i], p->id);
	}




	/* Wait for all children to raise SIGSTOP before exec()ing. */
	wait_for_ready_children(nproc);

	/* Install SIGALRM and SIGCHLD handlers. */
	install_signal_handlers();

	printf("*** SCHEDULER: dispatching the first process... ***\n");
	kill(p_list->head->pid, SIGCONT);
	alarm(SCHED_TQ_SEC);


	/* loop forever  until we exit from inside a signal handler. */
	while (pause())
		;

	/* Unreachable */
	fprintf(stderr, "Internal error: Reached unreachable point\n");
	return 1;
}
