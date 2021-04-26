#include <tty.h>
#include <io.h>
#include <topbar.h>


//*********************
// INICIALITZACIONS
//*********************

void init_tty (struct tty* tty) {
  tty->x = 0;
  tty->y = 0;
  
  tty->current_fg_color = 0xF; // White
  tty->current_bg_color = 0x0; // Black
  
  for (int row = 0; row < NUM_ROWS; row++) {
    for (int col = 0; col < NUM_COLUMNS; col++) {
      tty->buffer[row * NUM_COLUMNS + col] = 0x0F00;
    }
  }
}

void init_ttys_table() {
  
  // By default, there's only one tty
  
  ttys_table.focus = 0;
  ttys_table.used[0] = 1;

  struct tty* tty0 = & ttys_table.ttys[0];
  
  init_tty (tty0);
  
}


//*********************
// SET TTY ATTRIBUTES
//*********************

void set_current_cursor (int x, int y) {
  /*printk("CURSOR");
  printk(" -> x=");
  char aux[] = "_"; aux[0] = '0' + x;
  printk(aux);
  printk(" ; y=");
  char aux2[] = "_"; aux2[0] = '0' + y;
  printk(aux2);*/
}

void set_current_bg_color (int c) {
  /*printk("BG_COLOR");
  printk(" -> color=");
  char aux[] = "_"; aux[0] = '0' + (char) c;
  printk(aux);*/
}

void set_current_fg_color (int c) {
  /*printk("FG_COLOR");
  printk(" -> color=");
  char aux[] = "_"; aux[0] = '0' + (char) c;
  printk(aux);*/
}

void set_current_general_attr (int n) {
  /*printk("GENERAL_ATTR");
  printk(" -> n=");
  char aux[] = "_"; aux[0] = '0' + n;
  printk(aux);*/
}


//*********************
//  TTY CHANGE FOCUS
//*********************

int show_next_tty () {// Shift +TAB pressed (called in interrupt.c)
  printk("[Show next tty]\n");
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
  
  for (int row = 0; row < NUM_ROWS; row++) {
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

int sys_write_console (void* device, char* buffer, int size) {
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
    
    
    // TODO 
    // /// Cutre: write directe a pantalla
    // printc(buffer[i]);
    
    // Write al buffer del tty
    tty_printc (device, buffer[i]);
    
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


