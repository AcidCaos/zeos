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
  //printk(" --> sys_read_console()\n");
  char reading[CON_BUFFER_SIZE];
  int i = 0;
  
  //Avoid overflow. Space for the \0 at the end
  if (count >= CON_BUFFER_SIZE) count = CON_BUFFER_SIZE - 1;
  
  // Pop chars from console buffer
  while (i < count) {
    //printk(" --> sys_read_console() : Check if buffer is empty.\n");
    if (cyclic_buffer_is_empty(&console_input)) { // Nothing to read yet. Go to read queue
      //printk(" --> sys_read_console() : Nothing to read.\n");
      push_task_struct (current(), &read_queue);
      sched_next_rr(); // TODO : problema: no surt d'aquí
      //printk(" --> sys_read_console() : HERE!. Someone clicked a key??\n");
      continue; // re-start the loop
    }
    //If here, means there is something to read in the buffer
    //printk(" --> sys_read_console() : New character to read.\n");
    //if (cyclic_buffer_is_empty(&console_input))
      //printk ("Empty! before pop \n");
    char pop = cyclic_buffer_pop(&console_input); // TODO Això peta en l'últim caràcter... Revisar
    //printk ("OK pop \n");
    reading[i] = pop;
    //printk ("OK reading[i] \n");
    i++;
    //printk ("OK end while \n");
  }
  //printk(" --> sys_read_console() : Read all 'i' characters.\n");
  reading[i] = '\0';
  //printk(" ### SENDING BACK:");
  //printk(reading);
  // Finished: read 'count' chars
  copy_to_user(reading, user_buff, i+1);
  //printk(" ### USER BUFF:");
  //printk(user_buff);
  //printk(" --> sys_read_console() EOF\n");
  return i; // The number of bytes read is returned.
}





