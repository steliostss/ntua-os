#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <assert.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdbool.h>

#include "proc-common.h"
#include "request.h"
#include "functions.h"

/* Compile-time parameters. */

/* time quantum */
#define SCHED_TQ_SEC 2

/* maximum size for a task's name  */
#define TASK_NAME_SZ 60

/* exetutable for shell */
#define SHELL_EXECUTABLE_NAME "shell"

process* current_p;
process_list* l;


/* Print a list of all tasks currently being scheduled.  */
static void sched_print_tasks(void) {
	printf("\n***THE LIST***");
	print_list(l, current_p);
}
/* Send SIGKILL to a task determined by the value of its
 * scheduler-specific id.
 */
static int sched_kill_task_by_id(int id) {
	printf("\n\nATTEMPTING TO KILL THE PROCESS: %d\n", id);
	process* p = get_proc_by_id(l, id);

	if (p == NULL) {
		printf("Process not exists ins scheduler list\n");
		return 1;
	}

	printf("Process found is scheduler's list, executing SIGKILL\n");
	kill(p->pid, SIGKILL);
	return 0;
}
/* Create a new task.  */
static void sched_create_task(char *executable) {
	green();
	printf("\n\nATTEMPING TO CREATE THE PROCESS FOR: %s\n", executable);
	pid_t pid = fork();
	if (pid < 0) {
		perror("fork");
		exit(1);
	}
	if (pid == 0) {
		raise(SIGSTOP);
		char filepath[TASK_NAME_SZ];
		sprintf(filepath, "./%s", executable);
		// TODO
		char* args[] = {filepath, NULL};
		if (execvp(filepath, args)) {
			perror("execvp");
			exit(1);
		}
	}
	waitpid(pid, NULL, WUNTRACED);
	process *p = process_create(pid, executable);

	// Push process in low list
	push(l, p);
	printf("SCHEDULER: Process [name]: %s [id]: %d was succesfully created. Added in LOW.\n",
		executable, p->id);
	reset();
}
/* Process requests by the shell.  */
static int process_request(struct request_struct *rq) {
	switch (rq->request_no) {
		case REQ_PRINT_TASKS:
			sched_print_tasks();
			return 0;

		case REQ_KILL_TASK:
			return sched_kill_task_by_id(rq->task_arg);

		case REQ_EXEC_TASK:
			sched_create_task(rq->exec_task_arg);
			return 0;

		default:
			return -ENOSYS;
	}
}
/*
 * SIGALRM handler
 */
static void sigalrm_handler(int signum) {
	red();
	printf("\n*** SCHEDULER: Going to stop process [id]: %d\n",
			current_p->id);
	reset();
	kill(current_p->pid, SIGSTOP);
}
/*
 * SIGCHLD handler
 */
static void sigchld_handler(int signum) {
	printf("Signum: %d, pid: %ld\n", signum, (long)getpid());
	bool pass_to_next = false;
	int status;
	pid_t pid;
	for (;;) {
		pid = waitpid(-1, &status, WUNTRACED | WNOHANG);
		if (pid == 0) {
			break;
		}
		if (pid < 0) {
			perror("waitpid");
			exit(1);
		}
		if (pid != 0) {

			// Check if head process changed status
			process *p;
			red();

			// Process has stopped
			if (WIFSTOPPED(status)) {
				if (pid == (current_p->pid)) {
					red();
					printf ("*** SCHEDULER: STOPPED: Current Process [name]: %s [id]: %d\n",
						current_p->name, current_p->id);
					reset();
					p = get_next(l);
					pass_to_next = true;
				} else {
					process* affected = get_proc_by_pid(l, pid);
					if (affected != NULL) {
						red();
						printf ("*** SCHEDULER: STOPPED: NOT current Process [name]: %s [id]: %d\n",
							affected->name, affected->id);
						reset();
					} else {
						perror("\nTHIS SHOULD !NOT HAPPEN!\n");
					}
				}

			// Process has exited
			} else if (WIFEXITED(status)) {
				if (pid == (current_p->pid)) {
					printf ("*** SCHEDULER: EXITED: Current Process [name]: %s [id]: %d\n",
					current_p->name, current_p->id);
					erase_proc_by_id(l, current_p->id);
					free_process(current_p);

					if (empty(l)) {
						printf ("*** SCHEDULER: No more processes to schedule. Cleaning and exiting...\n\n");
						reset();
						exit(0);
					}
					p = l->head;
					pass_to_next = true;

				} else {
					process* affected = get_proc_by_pid(l, pid);
					if (affected != NULL) {
						printf ("*** SCHEDULER: EXITED: NOT Current Process [name]: %s [id]: %d\n",
								affected->name, affected->id);

						affected = erase_proc_by_pid(l, pid);
						free_process(affected);
					} else {
						perror("\n\nTHIS SHOULD NOT HAPPEN!\n\n\n");
						exit(11);
					}
				}
			} else if (WIFSIGNALED(status)) {
				if (pid == (current_p->pid)) {
					printf ("*** SCHEDULER: GOT KILLED: Current Process [name]: %s [id]: %d\n",
					current_p->name, current_p->id);
					p = pop(l);
					free_process(p);

					if (empty(l)) {
						printf ("*** SCHEDULER: No more processes to schedule. Cleaning and exiting...\n");
						exit(0);
					}

					p = l->head;
					pass_to_next = true;
				} else {
					process* affected = get_proc_by_pid(l, pid);
					if (affected != NULL) {
						printf ("*** SCHEDULER: GOT KILLED: NOT Current Process [name]: %s [id]: %d\n",
								affected->name, affected->id);

						affected = erase_proc_by_pid(l, pid);
						free_process(affected);
					} else {
						printf("\n\nTHIS SHOULD NOT HAPPEN WHEN SIGNALED\n\n\n");
						exit(11);
					}
				}
				reset();
			}
			else {
				red();
				printf("Something really strange happened!\n");
				reset();
				exit(100);
			}
			reset();
			if (pass_to_next) {
				printf ("*** SCHEDULER: Next process to continue: [name]: %s [id]: %d\n\n",
					p->name, p->id);
				current_p = p;
				kill(p->pid, SIGCONT);
				alarm(SCHED_TQ_SEC);
			}
		}
	}
}
/* Disable delivery of SIGALRM and SIGCHLD. */
static void signals_disable(void) {
	sigset_t sigset;
	sigemptyset(&sigset);
	sigaddset(&sigset, SIGALRM);
	sigaddset(&sigset, SIGCHLD);
	if (sigprocmask(SIG_BLOCK, &sigset, NULL) < 0) {
		perror("signals_disable: sigprocmask");
		exit(1);
	}
}
/* Enable delivery of SIGALRM and SIGCHLD.  */
static void signals_enable(void) {
	sigset_t sigset;
	sigemptyset(&sigset);
	sigaddset(&sigset, SIGALRM);
	sigaddset(&sigset, SIGCHLD);
	if (sigprocmask(SIG_UNBLOCK, &sigset, NULL) < 0) {
		perror("signals_enable: sigprocmask");
		exit(1);
	}
}
/* Install two signal handlers.
 * One for SIGCHLD, one for SIGALRM.
 * Make sure both signals are masked when one of them is running.
 */
static void install_signal_handlers(void) {
	sigset_t sigset;
	struct sigaction sa;
	sa.sa_flags = SA_RESTART;
	// Specify signals to be blocked while the handling funvtion runs
	sigemptyset(&sigset);
	sigaddset(&sigset, SIGCHLD);
	sigaddset(&sigset, SIGALRM);
	sa.sa_mask = sigset;
	sa.sa_handler = sigchld_handler;
	if (sigaction(SIGCHLD, &sa, NULL) < 0) {
		perror("sigaction: sigchld");
		exit(1);
	}
	// TODO In exercise the sa handler was reassigned, does it work?
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
static void do_shell(char *executable, int wfd, int rfd) {
	char arg1[10], arg2[10];
	char *newargv[] = { executable, NULL, NULL, NULL };
	char *newenviron[] = { NULL };

	sprintf(arg1, "%05d", wfd);
	sprintf(arg2, "%05d", rfd);
	newargv[1] = arg1;
	newargv[2] = arg2;

	raise(SIGSTOP);
	execve(executable, newargv, newenviron);

	/* execve() only returns on error */
	perror("scheduler: child: execve");
	exit(1);
}
/* Create a new shell task.
 *
 * The shell gets special treatment:
 * two pipes are created for communication and passed
 * as command-line arguments to the executable.
 */ static void sched_create_shell(char *executable, int *request_fd, int *return_fd) {
	pid_t p;
	int pfds_rq[2], pfds_ret[2];

	if (pipe(pfds_rq) < 0 || pipe(pfds_ret) < 0) {
		perror("pipe");
		exit(1);
	}

	p = fork();
	if (p < 0) {
		perror("scheduler: fork");
		exit(1);
	}

	if (p == 0) {
		/* Child */
		close(pfds_rq[0]);
		close(pfds_ret[1]);
		do_shell(executable, pfds_rq[1], pfds_ret[0]);
		assert(0);
	}
	/* Parent */
	process *proc = process_create(p, executable);
	push(l, proc);
	green();
	printf("Created process: SHELL: %s with pid: %ld\n",
			executable, (long)p);
	reset();

	waitpid(p, NULL, WUNTRACED);

	//wait_for_ready_children(1);

	close(pfds_rq[1]);
	close(pfds_ret[0]);

	*request_fd = pfds_rq[0];
	*return_fd = pfds_ret[1];
}
static void shell_request_loop(int request_fd, int return_fd) {
	int ret;
	struct request_struct rq;

	/*
	* Keep receiving requests from the shell.
	*/
	for (;;) {
		if (read(request_fd, &rq, sizeof(rq)) != sizeof(rq)) {
			perror("scheduler: read from shell");
			fprintf(stderr, "Scheduler: giving up on shell request processing.\n");
			break;
		}

		signals_disable();
		ret = process_request(&rq);
		signals_enable();

		if (write(return_fd, &ret, sizeof(ret)) != sizeof(ret)) {
			perror("scheduler: write to shell");
			fprintf(stderr, "Scheduler: giving up on shell request processing.\n");
			break;
		}
	}
}
int main(int argc, char *argv[]) {
	int nproc;
	/* Two file descriptors for communication with the shell */
	static int request_fd, return_fd;
	l = initialize_empty_list();

	/* Create the shell. */
	/* TODO: add the shell to the scheduler's tasks */
	sched_create_shell(SHELL_EXECUTABLE_NAME, &request_fd, &return_fd);

	/*
	* For each of argv[1] to argv[argc - 1],
	* create a new child process, add it to the process list.
	*/

	nproc = argc - 1; /* number of proccesses goes here */

	int i;
	for (i = 1; i < argc; i++) {
		pid_t pid;
		pid = fork();
		if (pid < 0) {
			perror("fork");
			exit(1);
		}
		if (pid == 0) {
			raise(SIGSTOP);
			char filepath[TASK_NAME_SZ];
			sprintf(filepath, "./%s", argv[i]);
			// TODO
			char* args[] = {filepath, NULL};
			if (execvp(filepath, args)) {
				perror("execvp");
				exit(1);
			}
		}

		process *p = process_create(pid, argv[i]);
		push(l, p);
		green();
		printf("Process name: %s id: %d is created.\n",
			argv[i], p->id);
		reset();
	}


	/* Wait for all children to raise SIGSTOP before exec()ing. */
	wait_for_ready_children(nproc);

	/* Install SIGALRM and SIGCHLD handlers. */
	install_signal_handlers();

	if (nproc == 0) {
		fprintf(stderr, "Scheduler: No tasks. Exiting...\n");
		exit(1);
	}

	printf("Scheduler dispatching the first process...\n");
	process* head = l->head;
	current_p = head;
	kill(head->pid, SIGCONT);
	alarm(SCHED_TQ_SEC);

	shell_request_loop(request_fd, return_fd);

	/* Now that the shell is gone, just loop forever
	* until we exit from inside a signal handler.
	*/

	reset();

	while (pause())
		;

	/* Unreachable */
	fprintf(stderr, "Internal error: Reached unreachable point\n");
	return 1;
}
