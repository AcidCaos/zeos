/*
 * sched.h - Estructures i macros pel tractament de processos
 */

#ifndef __SCHED_H__
#define __SCHED_H__

#include <list.h>
#include <types.h>
#include <mm_address.h>
#include <stats.h>
#include <tty.h>

#define NR_TASKS      		10
#define KERNEL_STACK_SIZE	1024

#define INIT_QUANTUM 		10  // Default Quantum (Quantum is inherited, and init is father of all)
#define IDLE_QUANTUM 		5   // idle Quantum

enum state_t { ST_RUN, ST_READY, ST_BLOCKED };

// TASK STRUCT
struct task_struct {
  int PID;			/* Process ID. This MUST be the first field of the struct. */
  struct list_head list_anchor; /* List head : This is the anchor (ancla) in the list */
  page_table_entry * dir_pages_baseAddr; /* Directory base address */
  unsigned long kernel_esp;     /* Position in the stack when last in inner_task_switch */
  int quantum;			/* Process total quantum */
  enum state_t state;		/* State of the process */
  struct stats stats;		/* Process rr-policy related statistics */
  
};

// TASK UNION
union task_union {
  struct task_struct task;
  unsigned long stack[KERNEL_STACK_SIZE]; /* pila de sistema, una per procés */
};

// task[] definition
extern union task_union task[NR_TASKS]; /* Vector de tasques */

// LISTS
struct list_head freequeue;
struct list_head readyqueue;
struct list_head blocked;


// FUNCIONS


#define KERNEL_ESP(t)       	(DWord) &(t)->stack[KERNEL_STACK_SIZE]

#define INITIAL_ESP       	KERNEL_ESP(&task[1])

/* Funció pel sys_fork */
int get_next_pid ();

/* Funcions per les qües */

struct task_struct* pop_task_struct (struct list_head* queue);
void push_task_struct (struct task_struct*, struct list_head* queue);

/* Inicialitza les dades del proces inicial */
void init_task1(void);
void init_idle(void);
void init_sched(void);
struct task_struct * current();
void task_switch(union task_union*t);
struct task_struct *list_head_to_task_struct(struct list_head *l);
int allocate_DIR(struct task_struct *t);
page_table_entry * get_PT (struct task_struct *t) ;
page_table_entry * get_DIR (struct task_struct *t) ;

/* Headers for the scheduling policy */
void scheduler();
void sched_next_rr();
void update_process_state_rr(struct task_struct *t, struct list_head *dest);
int needs_sched_rr();
void update_sched_data_rr();

#endif  /* __SCHED_H__ */
