#ifndef helper_h
#define helper_h

#include <unistd.h>
#include <stddef.h>

typedef struct process_type {
	pid_t pid;
	int id;
	char* name;
	struct process_type* next;
} process;

typedef struct process_list_type {
	process* head;
	process* tail;
	size_t size;
} process_list;

process* get_next(process_list* l);

process* process_create(pid_t pid, const char* name);

int empty(process_list* l);

process* pop(process_list* l);

int push(process_list* l, process* n);

process* get_proc_by_pid(process_list* l, int id);

process* get_proc_by_id(process_list* l, int id);

process* erase_proc_by_id(process_list* l, int id);

process* erase_proc_by_pid(process_list* l, int id);

void clear(process_list* l);

process_list* initialize_empty_list(void);

void free_process(process* p);

void print_list(process_list* l, process* current_p);

int empty_lists(process_list* l, process_list* h);

process* get_head_of_lists(process_list* l, process_list* h);

process* get_next_lists(process_list* l, process_list* h);

process* pop_list(process_list* l, process_list* h);

process* get_proc_by_pid_list(process_list* l, process_list * h, int id);

process* get_proc_by_id_list(process_list * l, process_list* h, int id);

process* erase_proc_by_id_list(process_list * l, process_list* h, int id);

process* erase_proc_by_pid_list(process_list* l, process_list* h, int id);

int move_from_to(process_list* a, process_list* b, int id);

void red();
void yellow();
void green();
void reset();

#endif
