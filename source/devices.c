#include <io.h>
#include <utils.h>
#include <list.h>

#include <sched.h>
#include <cyclic_buffer.h>
#include <devices.h>

// Queue for blocked processes in I/O 

// extern struct cyclic_buffer console_input; // devices.h


void init_devices() {
  INIT_LIST_HEAD ( &read_queue );
  init_cyclic_buffer(&console_input);
}



int sys_write_console (char *buffer, int size) {
  int i;
  
  for (i=0; i<size; i++)
    printc(buffer[i]);
  
  return size;
}

int sys_write_console_error (char *buffer, int size) {
  int i;
  
  for (i=0; i<size; i++)
    printc_error(buffer[i]);
  
  return size;
}

int sys_read_console (char* user_buff, int count) {
  
  char reading[CON_BUFFER_SIZE];
  int i = 0;
  
  //Avoid overflow. Space for the \0 at the end
  if (count >= CON_BUFFER_SIZE) count = CON_BUFFER_SIZE - 1;
  
  // Show writting '_' cursor
  print_text_cursor('_');
  
  // Pop chars from console buffer
  while (i < count) {
    if (cyclic_buffer_is_empty(&console_input)) { // Nothing to read yet. Go to read queue
      push_task_struct (current(), &read_queue);
      sched_next_rr();
      continue; // re-start the loop after returning from task_switch
    }
    //If here, means there is something to read in the buffer
    char pop = cyclic_buffer_pop(&console_input);
    reading[i] = pop;
    i++;
  }
  
  // reading[i] = '\0'; // No! Si lees en un solo char, se modificará el byte siguiente también con el /0.
  
  copy_to_user(reading, user_buff, i);
  
  return i; // The number of bytes read is returned.
}





