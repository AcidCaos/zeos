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

#define STDIN_FDNUM 0
#define STDOUT_FDNUM 1
#define STDERR_FDNUM 2


int check_fd (int fd, enum mode_t mode) {
  
  struct taula_canals* tc = &current()->taula_canals;
  struct canal_entry* e = &tc->taula_canals[fd];
  
  if (! e->used) return -EBADF; // EBADF: Bad file number
  if (! (e->mode == mode || e->mode == RDWR)) return -EACCES; // EACCES: Permission denied
  return 0;
}

int sys_ni_syscall() {
	return -ENOSYS; // ENOSYS: Function not implemented
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
  // init_stats(&fill_ts->stats); Done at the end.
  
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
  offset = PAG_LOG_INIT_DATA; // Numero pag. lògica inici de data+stack usuari
  unsigned long temp_offset = offset + NUM_PAG_DATA; // Després de les pags de Data, no s'usen aquelles pagines.
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
  fill_ts->quantum = current()->quantum;
  copy_taula_canals (& current()->taula_canals, & fill_ts->taula_canals);
  
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
  fill_ts->kernel_esp = (unsigned long) & (fill_tu->stack[fill_ebp_index - 1]);


  
  // (k) Prepare child for context switch
  fill_tu->stack[fill_ebp_index] = (unsigned long) & ret_from_fork;
  fill_tu->stack[fill_ebp_index - 1] = 0xCAFE;

  // (l) Init Child process stats
  init_stats(&fill_ts->stats);
  
  // (m) Put child to READY queue
  push_task_struct (fill_ts, &readyqueue);
  
  return PID;
}

int sys_write(int fd, char* buffer, int size) {

  char sys_buffer [1024];
  int ret;

  ret = check_fd(fd, WRONLY);
  if (ret != 0) return ret;
  if (buffer == NULL) return -14; /* EFAULT: Bad address */
  if (size <= 0 || size > 1024) return -22; /* EINVAL: Invalid argument */
  
  copy_from_user(buffer, sys_buffer, size); /* utils.c :: copy from user-buffer to system-buffer */
  
  /*if (fd == STDOUT_FDNUM) // stdout
  	ret = sys_write_console(sys_buffer, size); 
  else if (fd == STDERR_FDNUM) // stderr
  	ret = sys_write_console_error(sys_buffer, size); 
  */
  
  struct taula_canals* tc = &current()->taula_canals;
  void* device = tc->taula_canals[fd].device; // TODO : add device types, check which driver-function to call
  ret = sys_write_console(device, sys_buffer, size);
  
  return ret; // ret = num. of bytes written

}


int sys_get_stats(int pid, struct stats *s) {
  struct task_struct* t_s = NULL;
  // Check if PID is correct
  if (pid < 0) return -EINVAL; /* EINVAL: Invalid argument */
  for (int i = 0; i < NR_TASKS; ++i) {
    if (task[i].task.PID == pid) {
      t_s = &task[i].task; 
      break;
    }
  }
  if (t_s == NULL) return -ESRCH; /* ESRCH: No such process */
  // return stats
  copy_to_user (&t_s->stats, s, sizeof(struct stats));
  return 0;
}


int sys_read(int fd, char* user_buff, int count) {
  
  int ret;
  
  ret = check_fd(fd, RDONLY);
  if (ret != 0) return ret;
  if (user_buff == NULL) return -14; /* EFAULT: Bad address */
  
  if (count <= 0 || count >= 1024) return -22; /* EINVAL: Invalid argument */
  
  struct taula_canals* tc = &current()->taula_canals;
  void* device = tc->taula_canals[fd].device;
  ret = sys_read_console(device, user_buff, count); /* devices.c */
  
  return ret;
}

int sys_gettime () {
  return zeos_ticks;
}

int sys_open_tty_ro (int tty_fd) {
  
  int fd; // FD for Read
  
  struct taula_canals* tc = & current()->taula_canals;
  
  // Check taula canals no plena
  for (fd = 0; fd < MAX_CHANNELS; fd++) {
    if (! tc->taula_canals[fd].used) break;
  }
  if (fd == MAX_CHANNELS) return -EMFILE; /* EMFILE: Too many open files */
  
  // Get tty_fd's tty
  struct tty* tty = tc->taula_canals[tty_fd].device;
  if (tty == NULL) return -ENXIO; /* ENXIO: No such device or address */
  
  // Enllaçar fd amb el tty a la taula canals
  tc->taula_canals[fd].used = 1;
  tc->taula_canals[fd].mode = RDONLY;
  tc->taula_canals[fd].device = tty;
  
  return fd;
}

int sys_close (int fd) {
  
  struct taula_canals* tc = & current()->taula_canals;
  
  // Check real file descriptor
  if (! tc->taula_canals[fd].used) return -EBADF;	/* EBADF: Bad file number */
  
  // Delete from taula_canals
  tc->taula_canals[fd].used = 0;
  
  // Decrement tty use
  int ret = decrement_use_count_tty(tc->taula_canals[fd].device);
  
  return ret;
}

void sys_exit() {
  // (z) Close all opened files (tty's really)
  struct taula_canals* tc = & current()->taula_canals;
  for (int i = 0; i < MAX_CHANNELS; i++) {
    if (tc->taula_canals[i].used) sys_close(i);
  }
  
  // (a) Free the physical frames (for Data+Stack)
  unsigned long offset = NUM_PAG_KERNEL + NUM_PAG_CODE;
  page_table_entry* TP_proces = get_PT(current());
  for (int i = 0; i < NUM_PAG_DATA; i++) {
    free_frame(get_frame(TP_proces, offset + i));
    del_ss_pag(TP_proces, offset + i);
  }
  
  // (b) Free task struct
  push_task_struct(current(), &freequeue);
  
  // (c) Schedule next process
  sched_next_rr();
  
  errork("exit(): This point should never be reached.\n");
  return;
}

int sys_create_screen () {
  int fd;
  
  struct taula_canals* tc = & current()->taula_canals;
  
  // Check taula canals no plena
  for (fd = 0; fd < MAX_CHANNELS; fd++) {
    if (! tc->taula_canals[fd].used) break;
  }
  if (fd == MAX_CHANNELS) return -EMFILE; /* EMFILE: Too many open files */
  
  // Get unused tty
  struct tty* tty = get_init_free_tty ();
  if (tty == NULL) return -ENXIO; /* ENXIO: No such device or address */
  
  // Enllaçar fd amb el tty a la taula canals
  tc->taula_canals[fd].used = 1;
  tc->taula_canals[fd].mode = WRONLY;
  tc->taula_canals[fd].device = tty;
  
  return fd;
}

int sys_set_focus(int fd) {
  
  struct taula_canals* tc = & current()->taula_canals;
  struct tty* tty = tc->taula_canals[fd].device;
  
  if (! tc->taula_canals[fd].used) return -ENODEV; // No such device
  if (! (tc->taula_canals[fd].mode == WRONLY)) return -ENOTTY; // Not a typewriter
  
  for (int i = 0; i < MAX_TTYS; i++) {
    if ( & ttys_table.ttys[i] == tty) {
      
      return force_show_tty(i);
    }
  }
  return -ENODEV;
}


