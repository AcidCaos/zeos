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


int find_int(char* str, int* ptr) {
  int r = 0;
  int i = 0;
  while (str[i] >= '0' && str[i] <= '9') { // implicit check of end of string
    r = r * 10 + (int)(str[i] - '0');
    i++;
  }
  if (i == 0) return -1;
  *ptr += i;
  return r;
}

int sys_write_console (char *buffer, int size) {
  int i;
  
  for (i=0; i<size; i++) {
  
    
    // check for terminal escape codes
    // ###############################
    
    while (buffer[i] == '\033') { // Escape character
      
      int ii, ii_x1, x1, x2;
      ii = i + 1;
      
      while (buffer[ii] == ' ') ii++; // spaces
      if (buffer[ii] == '[') ii++;
      else break;
      while (buffer[ii] == ' ') ii++; // spaces
      ii_x1 = ii;
      x1 = find_int(buffer + ii, &ii);
      if (x1 < 0) break;
      while (buffer[ii] == ' ') ii++; // spaces
      if (buffer[ii] == ';') {
        ii++;
        while (buffer[ii] == ' ') ii++; // spaces
        x2 = find_int(&buffer[ii], &ii);
        if (x2 < 0) break;
        while (buffer[ii] == ' ') ii++; // spaces
        if (buffer[ii] == 0) break;
        if (buffer[ii] == 'H' || buffer[ii] == 'f') {
          ii++;
          i = ii; // jump all escape code
          
          
          set_current_cursor (x1, x2);
          
          
          break;
        } else break;
      }
      else if (buffer[ii] == 0) break;
      else {
        ii = ii_x1;
        if (buffer[ii] >= '0' && buffer[ii] <= '9') {
          x1 = (int)(buffer[ii] - '0');
          ii++;
        } else break;
        while (buffer[ii] == ' ') ii++; // spaces
        
        if (buffer[ii] >= '0' && buffer[ii] <= '9') {
          x2 = (int)(buffer[ii] - '0');
          ii++;
        }
        else if (buffer[ii] == 'm') {
          ii++;
          i = ii; // jump all escape code
          
          
          set_current_general_attr (x1);
          
          
          break;
        }
        else break;
        while (buffer[ii] == ' ') ii++; // spaces
        if (buffer[ii] == 'm') {
          ii++;
          i = ii; // jump all escape code
          
          
          if      (x1==3) set_current_fg_color(x2);
          else if (x1==4) set_current_bg_color(x2);
          
          
          break;
        }
        else break;
      }
    }
      
    // borrar caràcter actual ??
    
    // ###############################
    
    printc(buffer[i]);
  }
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
  print_text_cursor();
  
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





