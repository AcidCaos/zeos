#include <io.h>
#include <utils.h>
#include <list.h>

#include <sched.h>
#include <cyclic_buffer.h>
#include <devices.h>
#include <tty.h>

// Queue for blocked processes in I/O 
// extern struct cyclic_buffer console_input; // devices.h


void init_devices() {
  
  // ttys
  init_ttys_table();
  
}

void copy_taula_canals (struct taula_canals* from, struct taula_canals* to) {
  for (int i = 0; i < MAX_CHANNELS; i++) {
    to->taula_canals[i].used = from->taula_canals[i].used;
    if (! from->taula_canals[i].used) continue;
    to->taula_canals[i].mode = from->taula_canals[i].mode;
    to->taula_canals[i].device = from->taula_canals[i].device;

    increment_use_count_tty (to->taula_canals[i].device);
  }
}

void init_task1_std_io (struct taula_canals* tc) {
  // Std in
  tc->taula_canals[0].used = 1;
  tc->taula_canals[0].mode = RDONLY;
  tc->taula_canals[0].device = & ttys_table.ttys[0]; // Read
  // Std out
  tc->taula_canals[1].used = 1;
  tc->taula_canals[1].mode = WRONLY;
  tc->taula_canals[1].device = & ttys_table.ttys[0]; // Write
  // Std err
  tc->taula_canals[2].used = 1;
  tc->taula_canals[2].mode = WRONLY;
  tc->taula_canals[2].device = & ttys_table.ttys[0]; // Write Error
  
  for (int i = 3; i < MAX_CHANNELS; i++) {
    tc->taula_canals[i].used = 0;
  }
}

