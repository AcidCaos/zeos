/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>

union task_union task[NR_TASKS]
  __attribute__((__section__(".data.task")));

#if 0
struct task_struct *list_head_to_task_struct(struct list_head *l)
{
  return list_entry( l, struct task_struct, list);
}
#endif





// extern struct list_head blocked;
// Declare and Define Free queue and Ready queue
struct list_head freequeue;
struct list_head readyqueue;

struct task_struct* idle_task;

// Temp. for testing
struct task_struct* adam_task;




/* get_DIR - Returns the Page Directory address for task 't' */
page_table_entry * get_DIR (struct task_struct *t) 
{
	return t->dir_pages_baseAddr;
}

/* get_PT - Returns the Page Table address for task 't' */
page_table_entry * get_PT (struct task_struct *t) 
{
	return (page_table_entry *)(((unsigned int)(t->dir_pages_baseAddr->bits.pbase_addr))<<12);
}


int allocate_DIR(struct task_struct *t) 
{
	int pos;

	pos = ((int)t-(int)task)/sizeof(union task_union);

	t->dir_pages_baseAddr = (page_table_entry*) &dir_pages[pos]; 

	return 1;
}

void cpu_idle(void)
{
	__asm__ __volatile__("sti": : :"memory");

	
	while(1)
	{
	;
	}
}


// Pop a list_head from the free queue

struct task_struct* pop_free_task_struct () {

  struct list_head * first_elem = list_first( &freequeue );
  list_del (first_elem);
  return list_head_to_task_struct(first_elem);

}


void init_idle (void) {

  // Get available task_union from free queue
  union task_union* tu;
  struct task_struct* ts;
  ts = pop_free_task_struct();
  tu = (union task_union*) ts;

  // Assign PID = 0
  ts->PID = 0; // *(ts).PID = 0;

  // Inicialize dir_pages_baseAddr with a new directory
  allocate_DIR(ts);

  // Inicialize an execution context for the process to be restored when assigned to RUN
  //  *  Return address
  tu->stack[KERNEL_STACK_SIZE - 1] = (unsigned long) &cpu_idle;  // KERNEL_STACK_SIZE = 4Kbytes (1024 integers)
  //  *  Fake EBP
  tu->stack[KERNEL_STACK_SIZE - 2] = 0;
  //  *  task_struct kernel_esp
  ts->kernel_esp = (unsigned long) & tu->stack[KERNEL_STACK_SIZE - 2];

  // Inicialize idle_task
  idle_task = ts;
  
}

// Assembly code header includes
void writeMsr(unsigned long msr, unsigned long data); // entry.S


void init_task1 (void) { //(Task1 is Adam: antecessor of all processes)
  
  // Get available task_union from free queue
  union task_union* tu;
  struct task_struct* ts;
  ts = pop_free_task_struct();
  tu = (union task_union*) ts;

  // Assign PID = 1
  ts->PID = 1;

  // Inicialize dir_pages_baseAddr with a new directory
  allocate_DIR(ts);

  // Inicialization of address space // Allocate User Code and Data physical pages
  set_user_pages(ts);

  // Set the new system stack pointer
  //  *  Make TSS point to the bottom system stack
  tss.esp0 = (unsigned long) & tu->stack[KERNEL_STACK_SIZE];
  //  *  For sysenter: modify MSR 0x175
  writeMsr(0x175, (unsigned long) & tu->stack[KERNEL_STACK_SIZE]);
  
  // Set its page directory as the current page directory, setting register cr3
  set_cr3(ts->dir_pages_baseAddr);

  // Temp for testing
  adam_task = ts;
  
}

void init_sched () {

  // Inicialize Free Queue
  INIT_LIST_HEAD ( &freequeue );

  // Inicialize Ready Queue --> It is empty at the beginning
  INIT_LIST_HEAD ( &readyqueue );

  // Add all task structs from task[] to the Free Queue (all of them are available)
  for (int i = 0; i < NR_TASKS; i++) {
    list_add ( &task[i].task.list_anchor, &freequeue );
  }

  if (&task[NR_TASKS-1].task != list_head_to_task_struct(&task[NR_TASKS-1].task.list_anchor)) {
    // This should never happen
    errork ("Error: list_head_to_task_struct() is not working properly.\n");
  }

}



// Assembly code header includes from systemwrap.S
void task_switch (union task_union* new);
void asm_inner_task_switch (unsigned long* curr_k_esp, unsigned long new_k_esp);


void inner_task_switch (union task_union* new) {
  // Set the new system stack pointer
  //  *  Make TSS point to the bottom system stack
  tss.esp0 = (unsigned long) & new->stack[KERNEL_STACK_SIZE];
  //  *  For sysenter: also modify MSR 0x175
  writeMsr(0x175, (unsigned long) & new->stack[KERNEL_STACK_SIZE]);
  
  // Set new page directory as the current page directory, setting register cr3 (change current address space)
  set_cr3(new->task.dir_pages_baseAddr);
  
  // kernel_esp and other mafias
  asm_inner_task_switch ( (unsigned long *) & current()->kernel_esp, (unsigned long) new->task.kernel_esp);
  
}



struct task_struct *list_head_to_task_struct (struct list_head *l) {
  
  /*
   *  struct task_struct {
   *  ^  int PID;			
   *  |  page_table_entry * dir_pages_baseAddr; 
   *  L  struct list_head list; 
   *  };
   */
  
  // Given a pointer to list,
  // get a pointer to the tast_struct.

  // Here it is done by the hardcoded version: (return one int above)
  // return "the pointer @" - 1 * sizeof(int)
  // return (struct task_struct*) (l-1*sizeof(int));
  // return (struct task_struct*) ((char *)(l)-(unsigned long)( 1 * sizeof(int)));
  
  // Or could be done by the pre-processor:
  return ((struct task_struct *)((char *)(l)-(unsigned long)(&((struct task_struct *)0)->list_anchor)));
  
}

/*
 *   1 - The task_struct is located at the beginning of the page that it occupies.
 *   2 - To get a pointer to the task struct of the current process' task_struct.
 */

struct task_struct* current() {
  int ret_value;
  
  __asm__ __volatile__(
  	"movl %%esp, %0"
	: "=g" (ret_value)
  );
  return (struct task_struct*)(ret_value&0xfffff000);
}


//
//
//     SHEDULER
//
//

int aux_count = 0;
void scheduler () {
  aux_count++;
  if (aux_count%5 == 4) {
    if (current() == idle_task)  // Switches from idle <-to-> Adam forever. At least for now.
      task_switch((union task_union *) adam_task);
    else
      task_switch((union task_union *) idle_task);
  }
}

void sched_next_rr(){
}

void update_process_state_rr(struct task_struct *t, struct list_head *dest){
}

int needs_sched_rr(){
  return 0;
}

void update_sched_data_rr(){
}






