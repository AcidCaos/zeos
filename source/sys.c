/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>
#include <utils.h>
#include <io.h>
#include <mm.h>
#include <mm_address.h>
#include <sched.h>

#include <ticks.h>
#include <errno.h>

#define LECTURA 0
#define ESCRIPTURA 1


int check_fd(int fd, int permissions) {
  if (fd!=1) return -EBADF; /* EBADF: Bad file number */
  if (permissions!=ESCRIPTURA) return -EACCES; /* EACCES: Permission denied */
  return 0;
}

int sys_ni_syscall() {
	return -ENOSYS; /* ENOSYS: Function not implemented */
}

int sys_getpid() {
	return current()->PID;
}

// Include from systemwrap.S
int asm_get_ebp();

int ret_from_fork() {
  return 0;
}

int sys_fork() {

  int PID=-1;

  struct task_struct* fill_ts;
  union  task_union*  fill_tu;

  page_table_entry* TP_pare;
  page_table_entry* TP_fill;

  /*
     ** INDEX **
  
  === CHILD NEW PRIVATE PHYSICAL SPACE ===
  (a) Get a free task_struct for child
  (b) Copy task_struct to child --------------------------* System Stack        (private)
  (c) Allocate a new page-Directory for child
  (d) Allocate physical pages to map User Data+Stack of child
  (e) Copy User Data+Stack to child ----------------------* User Data + Stack   (private)

  === CHILD SHARED PHYSICAL SPACE ===
  (f) set System Code and Data TP entries to child -------* System Code + Data  (shared)
  (g) set User Code TP entries to child ------------------* User Code           (shared)
  (h) Flush TLB

  === CHILD CUSTOM TASK_STRUCT VALUES ===
  (i) Assign new PID to child
  (j) Map parent's ebp to child's stack
  (k) Prepare child for context switch
  (l) Init Child process stats
  (m) Put child to READY queue

  */
  
  // (a) Get a free task_struct for child
  // check no empty free queue. ENOMEM: Out of memory
  if (list_empty( &freequeue )) return -ENOMEM;
  fill_ts = pop_task_struct( &freequeue );
  fill_tu = (union task_union*) fill_ts;
  
  // (b) Copy task_struct to child ---------------------------* System Stack (private)
  // SYSTEM STACK: Copy parent's task_union to the child.
  // Child t_u already allocated, and in the TP.
  // copy_data((struct task_struct*) current(), (struct task_struct*) fill_ts, KERNEL_STACK_SIZE);
  copy_data((union task_union*) current(), (union task_union*) fill_ts, sizeof(union task_union));
  
  // (c) Allocate a new page-Directory for child -- Assignar directori, TP, al fill
  allocate_DIR(fill_ts);
  
  TP_pare = get_PT(current()); // NO!!--> [això és el directori. volem TP] current()->dir_pages_baseAddr;
  TP_fill = get_PT(fill_ts); // fill_ts->dir_pages_baseAddr;
  
  // (d) Allocate physical pages to map logic pages for User Data+Stack of child
  unsigned long offset = NUM_PAG_KERNEL + NUM_PAG_CODE;//PAG_LOG_INIT_DATA; // Numero pag. lògica inici de data+stack usuari
  for (int i = 0; i < NUM_PAG_DATA; i++) {
    unsigned long new_frame = alloc_frame(); // alloc_frame(): Search free physical page. mark it as USED_FRAME.
    if (new_frame < 0) return -ENOMEM; // TODO : deallocate frames. ENOMEM: Out of memory
    set_ss_pag(TP_fill, offset + i, new_frame); // Associates logical page of child with physical frame
  }
  
  // (e) Copy User Data+Stack to child allocated pages -------* User Data + Stack (private)
  offset = NUM_PAG_KERNEL + NUM_PAG_CODE;//PAG_LOG_INIT_DATA; // Numero pag. lògica inici de data+stack usuari
  unsigned long temp_offset = offset + NUM_PAG_DATA; // Després de les pags de Data, no s'usen aquelles pagines.
  printk("copy user data+stack start\n");
  for (int i = 0; i < NUM_PAG_DATA; i++) {
    unsigned long num_frame_fisic = get_frame(TP_fill, offset + i);
    // TP parent: temp. entry to map to child's physical frame, so we can access and modify them.
    set_ss_pag(TP_pare, temp_offset + i, num_frame_fisic);
    unsigned long from, to;
    from = (offset + i) << 12; // adreça inicial pàg. lògica del pare
    to = (temp_offset + i) << 12; // adr inicial pàg. lògica temp. del pare que apunta a fís. del fill
    //to = (temp_offset) << 12;
    
    copy_data((void *) from, (void *) to, PAGE_SIZE); // Accedeix a una entrada de la TP no allocatada.
    //copy_data((void *) from, (void *) to, 1); // Accedeix a una entrada de la TP no allocatada.
    del_ss_pag(TP_pare, temp_offset + i); // Undo temp. mapping
  }
  printk("copy user data+stack end\n");
  
  // (f) set System Code and Data TP entries to child --------* System Code + Data (shared)
  offset = (KERNEL_START >> 12); // Numero pag. lògica (i física) inici codi i dades kernel
  for (int i = 0; i < NUM_PAG_KERNEL; i++) {
    set_ss_pag(TP_fill, offset + i, get_frame(TP_pare, offset + i));
    //TP_fill[i + offset].entry = TP_pare[i + offset].entry;
  }
  
  // (g) set User Code TP entries to child -------------------* User Code (shared)
  offset = PAG_LOG_INIT_CODE; // Numero pag. lògica inici de codi usuari
  for (int i = 0; i < NUM_PAG_CODE; i++) {
    set_ss_pag(TP_fill, offset + i, get_frame(TP_pare, offset + i));
    //TP_fill[i + offset].entry = TP_pare[i + offset].entry;
  }
  
  // (h) Flush TLB
  set_cr3(current()->dir_pages_baseAddr); // És @ del directori, no de la TP
  
  // (i) Assign new PID to child and other task_struct data
  PID = get_next_pid(); // sched.h
  fill_ts->PID = PID;
  fill_ts->state = ST_READY;
  
  /*
  // (j) Map parent's ebp to child's stack
  unsigned long EBP_pare = asm_get_ebp(); // systemwrap.S
  int fill_ebp_index = (EBP_pare - (int)current() + (int)fill_ts);// / sizeof(int);
  // Això està malament calculat: (Surt 8fedc, hauria: ~1cfb8)
  fill_ts->kernel_esp = & (fill_tu->stack[fill_ebp_index - 1]); // Un més amunt!
  */
  // (j) Map parent's ebp to child's stack
  unsigned long EBP_pare = asm_get_ebp(); // systemwrap.S
  int fill_ebp_index = (EBP_pare - (int)current()) / sizeof(int);
  // Això està ben calculat: (Surt: ~1cfb4)
  fill_ts->kernel_esp = & (fill_tu->stack[fill_ebp_index - 1]);


  
  // (k) Prepare child for context switch
  fill_tu->stack[fill_ebp_index] = (unsigned long) & ret_from_fork;
  fill_tu->stack[fill_ebp_index - 1] = 0xCAFE;

  // (l) Init Child process stats
  // TODO
  
  // (m) Put child to READY queue
  push_task_struct (fill_ts, &readyqueue);
  
  return PID;
}

void sys_exit() {
}

int sys_write(int fd, char* buffer, int size) {

  char sys_buffer [1024];
  int ret;

  ret = check_fd(fd, ESCRIPTURA);
  if (ret != 0) return ret;
  if (buffer == NULL) return -14; /* EFAULT: Bad address */
  if (size <= 0) return -22; /* EINVAL: Invalid argument */

  copy_from_user(buffer, sys_buffer, size); /* utils.c :: copy from user-buffer to system-buffer */
  ret = sys_write_console(sys_buffer, size); /* devices.c */

  return ret; /* ret = num. of bytes written */

}

int sys_gettime () {
  return zeos_ticks;
}








