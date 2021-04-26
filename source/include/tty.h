#ifndef __TTY_H__
#define __TTY_H__


#define NUM_COLUMNS 80
#define NUM_ROWS    25

#define MAX_TTYS    30


struct tty {
  int x, y; // Cursor position and color
  char fg_color, bg_color;
  char buffer [NUM_COLUMNS * NUM_ROWS]; // Screen buffer
  
};

struct ttys_table {
  int used [MAX_TTYS];
  struct tty* ttys [MAX_TTYS];
  int focus;
  
};

struct ttys_table ttys_table;

void set_current_cursor (int x, int y);
void set_current_bg_color (int c);
void set_current_fg_color (int c);
void set_current_general_attr (int n);

int show_next_tty ();



#endif
