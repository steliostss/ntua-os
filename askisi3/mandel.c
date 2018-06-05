/*
 * mandel.c
 *
 * A program to draw the Mandelbrot Set on a 256-color xterm.
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#include "mandel-lib.h"

#define MANDEL_MAX_ITERATION 100000

/***************************
 * Compile-time parameters *
 ***************************/

/*
 * Output at the terminal is is x_chars wide by y_chars long
*/
int y_chars = 50;
int x_chars = 90;

/*
 * The part of the complex plane to be drawn:
 * upper left corner is (xmin, ymax), lower right corner is (xmax, ymin)
*/
double xmin = -1.8, xmax = 1.0;
double ymin = -1.0, ymax = 1.0;

/*
 * Every character in the final output is
 * xstep x ystep units wide on the complex plane.
 */
double xstep;
double ystep;

/*
 * An instance of this structure
 * is passed to every thread
 */

sem_t* sem;
int NTHREADS;

struct thread_info_struct {
	pthread_t thrid;	//POSIX thread id as return by the library
	int tnumber;		//e.g. thread no1
};

typedef struct thread_info_struct *thrptr;

int safe_atoi(char *s)
{
	long l;
	char* endp;

	l = strtol(s, &endp, 10);
	if (s != endp && *endp == '\0') {
		return l;
	}
	else
		return -1;
}

void *safe_malloc(size_t size)
{
        void *p;

        if ((p = malloc(size)) == NULL) {
                fprintf(stderr, "Out of memory, failed to allocate %zd bytes\n",
                        size);
                exit(1);
        }

        return p;
}

/*
 * This function computes a line of output
 * as an array of x_char color values.
 */
void compute_mandel_line(int line, int color_val[])
{
	/*
	 * x and y traverse the complex plane.
	 */
	double x, y;

	int n;
	int val;

	/* Find out the y value corresponding to this line */
	y = ymax - ystep * line;

	/* and iterate for all points on this line */
	for (x = xmin, n = 0; n < x_chars; x+= xstep, n++) {

		/* Compute the point's color value */
		val = mandel_iterations_at_point(x, y, MANDEL_MAX_ITERATION);
		if (val > 255)
			val = 255;

		/* And store it in the color_val[] array */
		val = xterm_color(val);
		color_val[n] = val;
	}
}

/*
 * This function outputs an array of x_char color values
 * to a 256-color xterm.
 */
void output_mandel_line(int fd, int color_val[])
{
	int i;

	char point = 164;
	char newline='\n';

	for (i = 0; i < x_chars; i++) {
		/* Set the current color, then output the point */
		set_xterm_color(fd, color_val[i]);
		if (write(fd, &point, 1) != 1) {
			perror("compute_and_output_mandel_line: write point");
			exit(1);
		}
	}

	/* Now that the line is done, output a newline character */
	if (write(fd, &newline, 1) != 1) {
		perror("compute_and_output_mandel_line: write newline");
		exit(1);
	}

	return;
}


void* thread_start_fn(void* arg)
{
	int line;
	int color_val[x_chars];

        /*
         * draw the Mandelbrot Set, one line at a time.
         * Output is sent to file descriptor '1', i.e., standard output.
         */

	thrptr thread = arg;

	//iterate lines with threads

        for (line = thread->tnumber; line < y_chars; line += NTHREADS) {
		compute_mandel_line(line, color_val);
		sem_wait(&sem[(line+NTHREADS)%NTHREADS]);	/*semaphore indicates thread to wait*/
		output_mandel_line(1, color_val);		/*when able to continue semaphore gets a signal to do so*/
		sem_post(&sem[(line+1+NTHREADS)%NTHREADS]);	/*wake next thread*/
	}

	return NULL;
}

int main(int argc, char* argv[])
{

	if (argc != 2){
		perror("Usage: ./mandel <N_THREADS>\nProvide exactly one argument.\n");
		exit(1);
	}

	NTHREADS = safe_atoi(argv[1]);
	int line;
	int ret;

	xstep = (xmax - xmin) / x_chars;
        ystep = (ymax - ymin) / y_chars;

	//checking threads
	if (NTHREADS <= 0) {
		fprintf(stderr, "%s not valid for 'NTHREADS'\n", argv[2]);
		exit(2);
	}

	printf("Running for N_THREADS = %d\n", NTHREADS);

	//thread allocation
	thrptr threads = safe_malloc(NTHREADS * sizeof(*threads));
	printf("threads allocated\n");

	//semaphore allocation
	sem = safe_malloc(NTHREADS * sizeof(*sem));
	printf("sem allocated\n");
	//semaphore initializing
	for(line=1; line < NTHREADS; ++line){
		sem_init(&sem[line], 0, 0);
	}
	//first 0 indicates that semaphores can be shared between threads of procs
	//second 0 indicates the value the semaphore is initialized with

	//signal first sem, ("wake")
	sem_init(&sem[0], 0, 1);

	//create threads
	int i;
	for(i=0; i < NTHREADS; ++i){
		threads[i].tnumber = i;
		//printf("thread: %d created\n", i);
		/* Create new thread */
		ret = pthread_create(&threads[i].thrid, NULL, thread_start_fn, &threads[i]);
		if (ret) {
			perror("pthread_create\n");
			exit(42);
		}
	}

	for(i=0; i < NTHREADS; ++i){
		ret = pthread_join(threads[i].thrid, NULL);
		if(ret) {
			perror("pthread_join\n");
			exit(420);
		}
	}

	reset_xterm_color(1);
	return 0;
}
