#include <tty.h>
#include <io.h>
#include <topbar.h>
#include <errno.h>
#include <sched.h>
#include <cyclic_buffer.h>
#include <utils.h>

//*********************
// INICIALITZACIONS
//*********************

void init_tty (struct tty* tty) {
  // General
  tty->pid_maker = current()->PID;
  
  // Output
  tty->x = 0;
  tty->y = topbar_enabled;
  
  tty->current_blinking = 0;
  tty->current_fg_color = 0xF; // White
  tty->current_bg_color = 0x0; // Black
  
  for (int row = 0; row < NUM_ROWS; row++) {
    for (int col = 0; col < NUM_COLUMNS; col++) {
      tty->buffer[row * NUM_COLUMNS + col] = 0x0F00;
    }
  }
  
  // Input
  INIT_LIST_HEAD ( & tty->read_queue );
  init_cyclic_buffer( & tty->console_input );
  
}

void init_ttys_table() {

  // By default, there's only one tty
  
  ttys_table.focus = 0;
  ttys_table.use_count[0] = 1;

  struct tty* tty0 = & ttys_table.ttys[0];
  
  init_tty (tty0);
  
  // Force its creator to be task1
  tty0->pid_maker = 1;
  
}

struct tty* get_init_free_tty () {
  
  for (int i = 0; i < MAX_TTYS; i++) {
    if ( ! ttys_table.use_count[i]) {
      ttys_table.use_count[i] = 1; // Set to used by 1
      init_tty(& ttys_table.ttys[i]); // Init attributes
      return & ttys_table.ttys[i];
    }
  }
  return NULL;
}

int increment_use_count_tty (struct tty* tty) {
  
  for (int i = 0; i < MAX_TTYS; i++) {
    if ( & ttys_table.ttys[i] == tty) {
      ttys_table.use_count[i]++;
      return 0;
    }
  }
  return -ENODEV; // No such device
}

int decrement_use_count_tty (struct tty* tty) {
  
  for (int i = 0; i < MAX_TTYS; i++) {
    if ( & ttys_table.ttys[i] == tty) {
      ttys_table.use_count[i]--;
      return 0;
    }
  }
  return -ENODEV; // No such device
}

//*********************
// SET TTY ATTRIBUTES
//*********************

void set_tty_cursor (struct tty* tty, int x, int y) {
  
  if (x < topbar_enabled || x >= NUM_ROWS || y < 0 || x >= NUM_COLUMNS)  return;
  
  tty->x = x;
  tty->y = y;
  
}

void set_tty_bg_color (struct tty* tty, int c) {

  //background : 0..7
  if (c < 0 || c > 7) return;
  
  tty->current_bg_color = c;
  
}

void set_tty_fg_color (struct tty* tty, int c) {

  //foreground : 0..F
  if (c < 0 || c > 0xF) return;
  
  tty->current_fg_color = c;
  
}

void set_tty_general_attr (struct tty* tty, int n) {
  
  if (n == 0) {
    tty->current_blinking = 0;
    tty->current_fg_color = 0xF; // White
    tty->current_bg_color = 0x0; // Black
  }
  
  else if (n == 5) tty->current_blinking = 1;
  
  else if (n == 9) tty->buffer[(tty->y * NUM_COLUMNS + tty->x)] = 0x00;
  
}


//*********************
//  TTY CHANGE FOCUS
//*********************

int show_next_tty () {// Shift +TAB pressed (called in interrupt.c)
  int current_focus = ttys_table.focus;
  for (int i = current_focus + 1; i < MAX_TTYS + current_focus; i++) {
    if (ttys_table.use_count[i % MAX_TTYS]) {
      ttys_table.focus = i % MAX_TTYS;
      return 0;
    }
  }
  return 0;
}

int force_show_tty (int i) {
  if (ttys_table.use_count[i]) ttys_table.focus = i;
  else return -ENODEV; // ENODEV: No such device
  return 0;
}

//*********************
// Show TTY to SCREEN
//*********************

void show_console () { // Called in clock_routine . TODO : FPS
  int focus = ttys_table.focus;
  struct tty* tty_focus = & ttys_table.ttys[focus];
  
  //Word* tty_buff = (Word *) & tty_focus->buffer; 
  Word* tty_buff = tty_focus->buffer;
  Word* screen = (Word *) 0xb8000;
  
  for (int row = topbar_enabled; row < NUM_ROWS; row++) {
    for (int col = 0; col < NUM_COLUMNS; col++) {
      
      // printc_xy (Byte mx, Byte my, char c);
      screen[row * NUM_COLUMNS + col] = tty_buff[row * NUM_COLUMNS + col];
      
    }
  }
  
}

//*********************
//  WRITE TO TTY
//*********************

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

int sys_write_console (struct tty* tty, char* buffer, int size) {
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
          
          
          set_tty_cursor (tty, x1, x2);
          
          
          continue;
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
        
        /*if (buffer[ii] >= '0' && buffer[ii] <= '9') {
          x2 = (int)(buffer[ii] - '0');
          ii++;
        }*/
        
        if (buffer[ii] >= '0' && buffer[ii] <= '9') {
          x2 = find_int(&buffer[ii], &ii);
        }
        
        else if (buffer[ii] == 'm') {
          ii++;
          i = ii; // jump all escape code
          
          
          set_tty_general_attr (tty, x1);
          
          
          continue;
        }
        else break;
        while (buffer[ii] == ' ') ii++; // spaces
        if (buffer[ii] == 'm') {
          ii++;
          i = ii; // jump all escape code
          
          
          if      (x1==3) set_tty_fg_color(tty, x2);
          else if (x1==4) set_tty_bg_color(tty, x2);
          
          
          continue;
        }
        else break;
      }
    }
      
    // borrar caràcter actual ??
    
    // ###############################
    
    
    // TODO 
    // /// Cutre: write directe a pantalla
    // printc(buffer[i]);
    
    // Write al buffer del tty
    tty_printc (tty, buffer[i]);
    
  }
  return size;
}

/*int sys_write_console_error (char *buffer, int size) { // TODO 
  int i;
  
  for (i=0; i<size; i++)
    printc_error(buffer[i]);
  
  return size;
}*/


//*********************
// TTY print to BUFFER
//*********************


void tty_printc (struct tty* tty, char c) {
  print_to_bochs(c);

  int x = tty->x;
  int y = tty->y;

  if (c=='\n') {
    x = 0;
    if (y + 1 >= NUM_ROWS) tty_scroll(tty);
    else y = y + 1;
  }
  else {
    
    Word fg_attr = (Word) (tty->current_fg_color & 0x000F);
    Word bg_attr = (Word) (tty->current_bg_color & 0x0007) << 4;
    Word blink_attr = (Word) (tty->current_blinking & 0x0001) << 7;
    
    Word attr_byte = (fg_attr | bg_attr | blink_attr) << 8;
    Word ch = (Word) (c & 0x00FF) | attr_byte;

    tty->buffer[(y * NUM_COLUMNS + x)] = ch;
    if (++x >= NUM_COLUMNS) {
      x = 0;
      if (y + 1 >= NUM_ROWS) tty_scroll(tty);
      else y = y + 1;
    }
  }
  
  tty->x = x;
  tty->y = y;
}

void tty_printk (char* str) {
  int i;
  struct tty* tty = & ttys_table.ttys[0];
  //struct tty* tty = & ttys_table.ttys[ttys_table.focus];
  int old_col = tty->current_fg_color;
  tty->current_fg_color = 0xA; // Green
  for (i = 0; str[i]; i++){
    tty_printc(tty, str[i]);
  }
  tty->current_fg_color = old_col;
}

void tty_scroll (struct tty* tty) { // if (topbar_enabled == 1) the TOP ROW should NOT be moved.
  
  // Move all up
  for (int row = 1 + topbar_enabled; row < NUM_ROWS; row++) {
    for (int col = 0; col < NUM_COLUMNS; col++) {
      tty->buffer[(row-1) * NUM_COLUMNS + col] = tty->buffer[row * NUM_COLUMNS + col];
    }
  }
  // Clean last row
  Word ch = (Word) (' ' & 0x00FF);
  for (int col = 0; col < NUM_COLUMNS; col++) {
    tty->buffer[(NUM_ROWS-1) * NUM_COLUMNS + col] = ch;
  }
}

void push_to_focus_read_buffer (char pr) {
  struct tty* tty = & ttys_table.ttys[ttys_table.focus];
  
  if (!cyclic_buffer_is_full( & tty->console_input)){
    cyclic_buffer_push ( & tty->console_input, pr);
    
    if (!list_empty( & tty->read_queue)) {
      struct task_struct* t_s_first = pop_task_struct( & tty->read_queue);
      update_process_state_rr(t_s_first, & readyqueue);
    } else {
    }
  }
  else errork(" --> keyboard_routine() : Read buffer is full!\n");
  
}

//*********************
//   READ from TTY
//*********************

int sys_read_console (struct tty* tty, char* user_buff, int count) {
  
  char reading[CON_BUFFER_SIZE];
  int i = 0;
  
  //Avoid overflow. Space for the \0 at the end
  if (count >= CON_BUFFER_SIZE) count = CON_BUFFER_SIZE - 1;
  
  // Show writting '_' cursor
  print_text_cursor();
  
  // Pop chars from console buffer
  while (i < count) {
    if (cyclic_buffer_is_empty( & tty->console_input)) { // Nothing to read yet. Go to read queue
      push_task_struct (current(), & tty->read_queue);
      sched_next_rr();
      continue; // re-start the loop after returning from task_switch
    }
    //If here, means there is something to read in the buffer
    char pop = cyclic_buffer_pop( & tty->console_input);
    reading[i] = pop;
    i++;
  }
  
  // reading[i] = '\0'; // No! Si lees en un solo char, se modificará el byte siguiente también con el /0.
  
  copy_to_user(reading, user_buff, i);
  
  return i; // The number of bytes read is returned.
}


